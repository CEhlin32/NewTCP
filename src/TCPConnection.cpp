#include <TCPConnection.h>    
#include <unistd.h>
#include <sys/socket.h>
#include <algorithm>
#include <MsgPacket.h>
#include <MyExceptions.h>
#include <MsgProcessor.h>
#include <MsgCmds.h>
#include <SystemMsgs.h>
#include <Autherization.h>
#include <Logger.h>
#include <DebugSupport.h>
#include <EnumExtender.h>

namespace CE::tcp
{
    std::vector<TCPConnection*> TCPConnection::m_ActiveConnections;

    TCPConnection::TCPConnection() :
        m_socket_fd(-1), 
        m_EncryptionEnabled(false),
        m_pReadThread(nullptr),
        m_KeepAlive(true)
    {
        m_ConnectionStatus.status = TCPConnectionStatus::Connection_Available;
        MsgProcessor::AddMsgProcessor(this);
        MsgProcessor::AddMsgCreator(this);
    }

    TCPConnection::~TCPConnection()
    {
        auto it = std::find(m_ActiveConnections.begin(), m_ActiveConnections.end(), this);
        if (it != m_ActiveConnections.end()) {
            m_ActiveConnections.erase(it);
        }

        if (m_pReadThread)
        {
            delete m_pReadThread;
            m_pReadThread = nullptr;
        }
    }      

    bool TCPConnection::StartReadThread()
    {
        m_pReadThread = new NamedThread("TCPConnectionReadThread",
            &TCPConnection::ReadThreadFunction, this);
        m_ReadThreadId = m_pReadThread->get_id();

        return true;
    }

    void TCPConnection::StopReadThread()
    {
        m_KeepAlive = false;
        close(m_socket_fd);
        if (m_pReadThread != nullptr)
        {
            m_pReadThread->join();
            delete m_pReadThread;
            m_pReadThread = nullptr;
        }
    }   

    TCPConnectionStatus::ServerStatus TCPConnection::GetConnectionStatus() const
    {
        return m_ConnectionStatus.status;
    }
    
    void TCPConnection::ReadThreadFunction()
    {
        m_ActiveConnections.push_back(this);
        // Send AvailableCmdInfoMsg to client
        AvailableCmdInfoMsg cmdInfoMsg;
        Send(cmdInfoMsg);
        // Implementation for reading data from the socket
        while(m_KeepAlive == true)
        {
            char buffer[1024];
            // Read Msg header
            MsgPacket packet;
            packet.SetConnectionID(m_socket_fd); // Set connection ID if needed
            if(packet.GetMsgHdrSize() != read(m_socket_fd, packet.GetMsgHdrDataPtr(),packet.GetMsgHdrSize()))
            {
                // Handle read error or disconnection
                break;
            }
        
            // Read Msg body using size from header
            int sizeToRead = packet.GetMsgBodySize();

            if(sizeToRead > 0)
            {
                if(true == packet.GetIsEncrypted())
                {
                    sizeToRead = ((sizeToRead + 15) / 16) * 16; // AES block size is 16 bytes
                }
                std::vector<uint8_t> encryptedData(sizeToRead);
                if(sizeToRead != read(m_socket_fd, encryptedData.data(), sizeToRead))
                {
                    // Handle read error or disconnection
                    break;  
                }
                packet.SetPacketDataFromBytes(encryptedData);
                if(true == packet.GetIsEncrypted())
                {
                    // Decrypt Msg here
                    DecryptMsg(packet);
                }
                else
                {
                    packet.SetBodyDataFromBytes(packet.GetPacketDataAsBytes());
                }

                bool msgProcessed = MsgProcessor::ProcessMsgFromPacket(packet);
            }
        }
    }

    Msg* TCPConnection::CreateMsgFromPacketInternal(MsgPacket& packet)
    {
        switch(packet.GetRelID())
        {
            case EnableEncryptDecryptCmd:
                return  new EnableEncryptDecryptMsg(packet);
            case RequestKeyAndIVCmd:
                return new RequestKeyAndIVMsg(packet);
            case UpdateKeyAndIVCmd:
                return new UpdateKeyAndIVMsg(packet); 
            case KeepAliveCmd:
                return new KeepAliveMsg(packet);
            case RemoteConnectionOkCmd:
                return new RemoteConnectionOkMsg(packet);
            case AutherizationStartRequestCmd:
                return new AutherizationStartRequestMsg(packet);
            case AutherizationReadyCmd:
                return new AutherizationReadyMsg(packet);
            case ValidateIVCmd:
                return new ValidateIVMsg(packet);
            case ValidateIVResultCmd:
                return new ValidateIVResultMsg(packet);
            case DebugQueryCmd:
                return new DebugQueryMsg(packet);
            case DebugResponseCmd:
                return new DebugResponseMsg(packet);
            
            default:
                break;
        }
        return nullptr;
    }

    bool TCPConnection::ProcessMsg(Msg& msg)
    {
        int absMessageID = msg.GetMsgID();
        int relMessageID = CE::tcp::TCPMsgEnumManager::Get().GetRelativeID(absMessageID);
        switch(relMessageID)
        {
            case EnableEncryptDecryptCmd:
            {               
                EnableEncryptDecryptMsg& enableEncryptDecryptMsg = (EnableEncryptDecryptMsg&)msg;
                EnableEncryption(enableEncryptDecryptMsg.m_nextIV, enableEncryptDecryptMsg.GetEncryptionIV());
                
                // send msg info data
                
                break;
            }
            case AutherizationStartRequestCmd:
            {
                AutherizationStartRequestMsg& authReqMsg = (AutherizationStartRequestMsg&)msg;
                Autherization * pAuth = Autherization::Get();
                pAuth->StartAutherizationMode(authReqMsg.GetInitValue());

                // Send AutherizationReadyCmd ready
                AutherizationReadyMsg readyMsg;
                Send(readyMsg);

                CryptoBlockVector iv = pAuth->GetIV();
                CryptoBlockVector key = pAuth->GetKey();
                m_aes_encrypt.AES_init_ctx_iv(key, iv);
                m_aes_decrypt.AES_init_ctx_iv(key, iv);
                break;
            }
            case ValidateIVCmd:
            {
                // Handle ValidateIV command
                ValidateIVMsg& validateMsg = (ValidateIVMsg&)msg;
                CryptoBlockVector ivToValidate = validateMsg.GetIVToValidate();

                bool ivIsValid = true;
                CryptoBlockVector key = AESAccessManagement::Get()->GetASEKey(ivToValidate);
                if (key.size() == 0)
                {
                    ivIsValid = false;
                    TheAppLogger.LogMsgWithTime(DebugErrorLogOption::instance(),
                         "Failed to find key for iv = %s\n", 
                         DebugSupport::ByteToCharArray(ivToValidate.data(), ivToValidate.size()).c_str());
                    // TODO make byte to char array to a string passed as argument
                }
                if(ivIsValid == true)
                {
                    m_aes_decrypt.AES_init_ctx_iv(key, ivToValidate);
                    m_aes_encrypt.AES_init_ctx_iv(key, ivToValidate);
                    AESCryptor::printArray(m_aes_decrypt.GetKey().data(), 16);
                    AESCryptor::printArray(m_aes_decrypt.GetIV().data(), 16);
                }
                // Send ValidateIVResultCmd result
                ValidateIVResultMsg resultMsg;
                resultMsg.SetIsValid(ivIsValid);    
                Send(resultMsg);
                break;
            }
            case DebugQueryCmd:
            {
                DebugQueryMsg& debugQueryMsg = (DebugQueryMsg&)msg;
                // just respond with debug response
                CommandDefinition* pCmd = CmdDefinitionManager::GetCommandDefinitions(debugQueryMsg.GetQueryStr());
                DebugResponseMsg debugRespMsg;
                if(pCmd != nullptr)
                {
                    pCmd->Action();
                }
                else
                {
                    DebugResponseMsg debugRespMsg;    
                    debugRespMsg.SetResponseStr("Debug Response: Unknown Command");
                    Send(debugRespMsg);
                }
            }
            break;
            default:
                break;
        }   
    }


    int TCPConnection::SendToAll(Msg& msg)
    {
        int result = 0;
        for(auto connection : m_ActiveConnections)
        {
            int sendResult = connection->Send(msg);
            if(sendResult != 0)
            {
                result = sendResult; // Return the last error encountered
            }
        }
        return result;
    }

    int TCPConnection::Send(Msg& msg)
    {
        try
        {
            if(m_socket_fd < 0)
            {
                return -1; // Invalid socket
            }
            /* code */
            MsgPacket& packet = msg.Serialize();
            packet.SetConnectionID(m_socket_fd); // Set connection ID if needed

            if(m_EncryptionEnabled)
            {
                // Encrypt msg here
                EncryptMsg(packet);
            }
            int hdrSize = packet.GetMsgHdrSize();
            if(packet.GetMsgHdrSize() != send(m_socket_fd, packet.GetMsgHdrDataPtr(),packet.GetMsgHdrSize(), 0))
            {
                return -2; // Send failed
            }
            if(packet.GetPacketDataAsBytes().size() > 0)
            {
                // Send Msg body here
                std::vector<uint8_t> bd = packet.GetPacketDataAsBytes();
                if(packet.GetPacketDataAsBytes().size() != 
                    send(m_socket_fd, packet.GetPacketDataAsBytes().data(), packet.GetPacketDataAsBytes().size(), 0))
                {
                    return -3; // Send body failed
                }
            }
        }
        catch(const MyExceptions::IOException& e)
        {
            return -1; // Exception occurred
        }
        return 0; // Success

    }
    bool TCPConnection::IsEncryptionEnabled() const
    {
        return m_EncryptionEnabled;
    }
    int TCPConnection::GetSocketFd() const
    {
        return m_socket_fd;
    }

    void TCPConnection::SetSocketFd(int socketFd)
    {
        m_socket_fd = socketFd;
    }

    void TCPConnection::EnableEncryption(const CryptoBlockVector &iv, const CryptoBlockVector &key)
    {
        m_aes_encrypt.AES_init_ctx_iv(key, iv);
        m_aes_decrypt.AES_init_ctx_iv(key, iv);
        m_EncryptionEnabled = true;
    }
    bool TCPConnection::EncryptMsg(MsgPacket& packet)
    {
        // Implementation for encrypting a message
        m_aes_encrypt.AES_CBC_encrypt_buffer(packet.GetBodyDataAsStr(),packet.GetPacketDataAsBytes());
        return true;
    }
    bool TCPConnection::DecryptMsg(MsgPacket& packet)
    {
        // Implementation for decrypting a message
        m_aes_decrypt.AES_CBC_decrypt_buffer(packet.GetPacketDataAsBytes(), packet.GetBodyDataAsBytes());


        return true;
    }
} // namespace CE::tcp

