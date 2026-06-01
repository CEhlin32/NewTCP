#include <TCPConnection.h>    
#include <unistd.h>
#include <sys/socket.h>
#include <algorithm>
#include <MsgPacket.h>
#include <MyExceptions.h>

#include <SystemMsgs.h>
#include <Autherization.h>
#include <Logger.h>
#include <DebugSupport.h>
#include <EnumMsgIDMgr.h>
#include <SystemMsgConstants.cs.h>
#include <MsgManager.h>

namespace CE::tcp
{
    std::vector<TCPConnection*> TCPConnection::m_ActiveConnections;
    PublisherBase TCPConnection::m_Publisher;

    TCPConnection::TCPConnection() : 
        m_pReadThread(nullptr),
        m_ReadThreadId(0),
        m_ConnectionStatus(),
        m_Pairing(false),
        EncryptionSetupComplete(false),
        m_socket_fd(-1),
        m_aes_encrypt("TCPConnection_AESEncryptor"),
        m_aes_decrypt("TCPConnection_AESDecryptor"),
        m_Pairing_aes_encrypt("TCPConnection_Pairing_AESEncryptor"),
        m_Pairing_aes_decrypt("TCPConnection_Pairing_AESDecryptor"),
        m_SendMutex(), // Mutex for synchronizing access to the send function
        m_KeepAlive(false)
    {
        m_ConnectionStatus.status = TCPConnectionStatus::Connection_Available;
        MsgManager::GetInstance().RegisterMsgNames(SharedSysMsgConstants::SystemCmdNames);
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

    int TCPConnection::SubscribeToConnectionComplete(SubscriberBase *subscriber)
    {
        return m_Publisher.Subscribe(subscriber);
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
        m_ActiveConnections.erase(std::remove(m_ActiveConnections.begin(), m_ActiveConnections.end(), this), m_ActiveConnections.end());
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
    

    int TCPConnection::ReadNoBodyMsg(Msg& msg, MsgPacket& packet)
    {
        // For messages with no body, 
        //we can directly create the Msg instance based on the header information
        msg = *MsgManager::GetInstance().CreateMsgFromPacket(packet);

        // returns  1 if suceesful.
        // -1, there was an error creating msg.
        if(msg.GetMsgID() == 0)
        {
            return -1; // Error creating message, invalid MsgID
        }   
        return 0; // Return 0 for success
    }
    int TCPConnection::ReadEncryptedBodyMsg(Msg& msg, MsgPacket& packet, size_t sizeToRead)
    {
        // For messages with an encrypted body, 
        //we need to read the body data, decrypt it, and then create the Msg instance
        Msg * pMsg = DecryptMsg(packet);
        msg = *pMsg;
        if(pMsg == nullptr)
        {
            return -1; // Error decrypting message or creating message instance
        }
        return 1; // Return 1 for success
    }

    int TCPConnection::ReadUnEncryptedBodyMsg(Msg& msg, MsgPacket& packet, size_t sizeToRead)
    {   
        // For messages with an unencrypted body, 
        //we can read the body data directly and then create the Msg instance
        std::vector<uint8_t> bodyData(sizeToRead);
        size_t sizeRead = read(m_socket_fd, bodyData.data(), sizeToRead);
        if(sizeRead != sizeToRead)
        {
            return -1; // Error reading body data
        }
        packet.SetPacketDataFromBytes(bodyData);
        Msg * pMsg = MsgManager::GetInstance().CreateMsgFromPacket(packet);
        msg = *pMsg;
        if(pMsg == nullptr)        {
            return -1; // Error creating message instance
        }
        return 1; // Return 1 for success
    }

    void TCPConnection::ReadThreadFunction()
    {
        m_ActiveConnections.push_back(this);
        m_ConnectionStatus.status = TCPConnectionStatus::Connection_Connected;
        m_KeepAlive = true;
        m_Pairing = false; // Reset pairing mode when starting read thread for a new connection
        EncryptionSetupComplete = false;
        Msg* pMsg = nullptr;
        
        // Implementation for reading data from the socket
        while(m_KeepAlive == true)
        {
            // Read Msg header
            MsgPacket packet;
            packet.SetConnectionID(m_socket_fd); // Set connection ID if needed
            size_t numBytesRead = read(m_socket_fd, packet.GetMsgHdrDataPtr(),packet.GetMsgHdrSize());
            if(packet.GetMsgHdrSize() != numBytesRead)
            {
                // Handle read error or disconnection
                break;
            }
            // make sure MsgID is valid
            if( MsgManager::GetInstance().IsValidID(packet.GetMsgID()) == false)
            {
                // Read Body if there is one
                size_t sizeToRead = packet.GetMsgBodySize();
                size_t sizeRead = 0;
                if( packet.GetIsEncrypted())
                {
                    sizeToRead = packet.GetMsgBodySize();
                    // If Msg is encrypted, we need to read the body data to get the full packet for decryption attempt, even if MsgID is invalid, to determine if failure is due to decryption failure or unknown MsgID
                    std::vector<uint8_t> bodyData(sizeToRead);
                    sizeRead = read(m_socket_fd, bodyData.data(), sizeToRead);
                }
                else
                {
                    std::vector<uint8_t> bodyData(sizeToRead);
                    sizeRead = read(m_socket_fd, bodyData.data(), sizeToRead);
                }

                CommunicationFailedMsg resultMsg;
                resultMsg.SetReasonCode(CommunicationFailedMsg::UnknownMsgID); // Set reason code for refusal
                resultMsg.SetMsgID(packet.GetMsgID()); // Set the MsgID that caused the failure
                Send(resultMsg);
                continue; // Continue to next iteration to keep connection alive for valid future messages
            }
        
            // Read Msg body using size from header
            size_t sizeToRead = packet.GetMsgBodySize();
            int result = 0;
            if(sizeToRead == 0)
            {
                result = ReadNoBodyMsg(*pMsg, packet);
            }
            else if(sizeToRead > 0)
            {
                if(packet.GetIsEncrypted())
                {
                    // If Msg is encrypted, need to adjust size to read to be modula 16
                    sizeToRead = sizeToRead + (16 - (sizeToRead % 16));
                }
                std::vector<uint8_t> bodyData(sizeToRead);
                size_t sizeRead = read(m_socket_fd, bodyData.data(), sizeToRead);
                if(sizeRead != sizeToRead)
                {
                    if(sizeRead != 0)
                    {
                        result = -1; // Error reading body data
                    }
                    result = 0; // client closed connection Return 0 for clean disconnect 

                }
                if(packet.GetIsEncrypted())
                {
                    packet.SetPacketDataFromBytes(bodyData);
                    result = ReadEncryptedBodyMsg(*pMsg, packet, sizeToRead);
                }
                else
                {
                    packet.SetPacketDataFromBytes(bodyData);
                    result = ReadUnEncryptedBodyMsg(*pMsg, packet, sizeToRead);
                }

            }
            if( pMsg != nullptr)
            {
                ProcessMsg(*pMsg);
                delete pMsg;
                pMsg = nullptr;
                continue;
            }
            // If we reach here, 
            //it means there was an issue with reading or processing the message, 
            //we can choose to break the loop or continue based on the type of error
            if(result == 0)
            {
                // Clean disconnect by client
                break;
            }
            continue;
        }

        m_KeepAlive = false;
        close(m_socket_fd);
        m_ActiveConnections.erase(std::remove(m_ActiveConnections.begin(), m_ActiveConnections.end(), this), m_ActiveConnections.end());
        m_ConnectionStatus.status = TCPConnectionStatus::Connection_Available;
    }



    bool TCPConnection::ProcessMsg(Msg& msg)
    {
        bool result = true;
        switch(msg.GetMsgID())
        {
            case MsgManager::IntHashOfStr("EnableEncryptDecryptCmd"):
            {               
                // send msg info data
                break;
            }
            case MsgManager::IntHashOfStr("AutherizationStartRequestCmd"):
            {
                m_Pairing = true;
                AutherizationStartRequestMsg& authReqMsg = (AutherizationStartRequestMsg&)msg;
                Autherization * pAuth = Autherization::Get();
                pAuth->StartAutherizationMode(authReqMsg.GetInitValue());

                // Enable Encryption and set Key and IV for this connection
                m_Pairing_aes_encrypt.AES_init_ctx_iv(pAuth->GetKey(), pAuth->GetIV());
                m_Pairing_aes_decrypt.AES_init_ctx_iv(pAuth->GetKey(), pAuth->GetIV());
                // Send AutherizationReadyCmd ready
                AutherizationReadyMsg readyMsg;
                Send(readyMsg);
                break;
            }

            case MsgManager::IntHashOfStr("AutherizationValidationCmd"):
            {
                AutherizationValidationMsg& authValidationMsg = (AutherizationValidationMsg&)msg;
                CryptoBlockVector ivToValidate = authValidationMsg.IV_TO_VALIDATE;
                CryptoBlockVector serverIV = Autherization::Get()->GetIV();
                bool ivsMatch = (serverIV == ivToValidate);
                // Send AutherizationValidationResponseMsg result
                AutherizationValidationResponseMsg validationRespMsg;
                validationRespMsg.IV_IS_VALID = ivsMatch;
                Send(validationRespMsg);
                if(ivsMatch)
                {
                    UpdateKeyAndIVMsg setKeyAndIVMsg;
                    Send(setKeyAndIVMsg);
                    CryptoBlockVector serverKey = Autherization::Get()->GetKey();
                    IVAndKeyValues keyAndIV(serverKey, ivToValidate);

                    m_aes_decrypt.AES_init_ctx_iv(setKeyAndIVMsg.GetKey(), setKeyAndIVMsg.GetIV());
                    m_aes_encrypt.AES_init_ctx_iv(setKeyAndIVMsg.GetKey(), setKeyAndIVMsg.GetIV());

                    // send End Validation
                    AutherizationEndMsg endAuthMsg;
                    Send(endAuthMsg);
                    // Stop autherization mode after validation result is sent
                    Autherization::Get()->StopAutherizationMode();
                }
                else
                {
                    TheAppLogger.LogMsgWithTime(DebugErrorLogOption::instance(),
                            "IV validation failed for connection ID = %d\n", 
                            m_socket_fd);
                }
                m_Pairing = false; // End pairing mode after validation attempt
                Autherization::Get()->StopAutherizationMode();
                break;
            }

            case MsgManager::IntHashOfStr("RequestKeyAndIVCmd"):
            {
                UpdateKeyAndIVMsg setKeyAndIVMsg;
                AESAccessManagement::Get()->AddASEKeyAndIV(setKeyAndIVMsg.GetIVAndKeyValues());
                Send(setKeyAndIVMsg);

            }
            break;

            case MsgManager::IntHashOfStr("ValidateIVCmd"):
            {
                // Handle ValidateIV command
                bool ivIsValid = true;

                ValidateIVMsg& validateMsg = (ValidateIVMsg&)msg;
                CryptoBlockVector ivToValidate = validateMsg.GetIVToValidate();

                if( m_Pairing == false)
                {
                    ivIsValid = AESAccessManagement::Get()->IsIVValid(ivToValidate);
                    // Send ValidateIVResultCmd result
                    ValidateIVResultMsg resultMsg;
                    resultMsg.SetIsValid(ivIsValid);    
                    Send(resultMsg);
                    if(ivIsValid == true)
                    {
                        // Sen AvailableCmd Info to Client
                        AvailableCmdInfoMsg availableCmdInfoMsg;
                        Send(availableCmdInfoMsg);
                        
                        // Notify subscribers that connection is complete and status can be sent
                        ConnectionCompleteData* eventData = new ConnectionCompleteData(m_socket_fd);
                        m_Publisher.PublishEvent(eventData); 

                    }
                }

                break;
            }

            case MsgManager::IntHashOfStr("AddToCmdInfoCmd"):
            {
                break;
            }
            case MsgManager::IntHashOfStr("DebugQueryCmd"):
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
                result = false;
                break;
        }   
        result |= MsgManager::GetInstance().ProcessMsgs(msg);
        return result;
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

            lock_guard<std::mutex> lock(m_SendMutex); // Ensure thread safety for sending messages
            /* code */
            MsgPacket& packet = msg.Serialize();
            packet.SetConnectionID(m_socket_fd); // Set connection ID if needed

            if(packet.GetIsEncrypted())
            {
                // Encrypt msg here
                EncryptMsg(packet);
            }
            else
            {
                // If not encrypted, we need to make sure the body data is set correctly in the packet for sending
                packet.SetBodyDataFromStr(packet.GetPacketDataAsStr());
            }
            if( -1 == packet.GetMsgID())
            {
                TheAppLogger.LogMsgWithTime(DebugErrorLogOption::instance(),
                    "Failed to serialize message %s for sending on connection ID = %d\n", 
                    msg.GetName().c_str(), m_socket_fd);
                return -1; // Serialization failed
            }
            size_t hdrSize = packet.GetMsgHdrSize();
            size_t sizeSent = send(m_socket_fd, packet.GetMsgHdrDataPtr(),packet.GetMsgHdrSize(), 0);
            if(hdrSize != sizeSent) 
            {
                return -2; // Send failed
            }
            if(packet.GetPacketDataAsBytes().size() > 0)
            {
                // Send Msg body here
                std::vector<uint8_t> bd = packet.GetPacketDataAsBytes();
                size_t bodySize = bd.size();
                size_t bodySent = send(m_socket_fd, bd.data(), bodySize, 0);
                if(bodySize != bodySent)
                {
                    return -3; // Send body failed
                }
            }
        }
        catch(const MyExceptions::IOException& e)
        {
            return -1; // Exception occurred
        }
        cout << "Sent Msg " + msg.GetName()  << endl;
        return 0; // Success

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
    }

    bool TCPConnection::EncryptMsg(MsgPacket& packet)
    {
        if(m_Pairing == false)
        {
            cout << "Encrypting NON PAIRING Msg " + packet.GetName() + " with : " << endl;
            m_aes_encrypt.PrintKeyAndIV();

            // During pairing mode, we use the IV and key set for this connection without trying to find a match
            m_aes_encrypt.AES_CBC_encrypt_buffer(packet.GetBodyDataAsStr(),packet.GetPacketDataAsBytes());

        }
        else
        {
            cout << "Encrypting PAIRING Msg " + packet.GetName() + " with : " << endl;
            m_Pairing_aes_encrypt.PrintKeyAndIV();

            // During pairing mode, we use the IV and key set for this connection without trying to find a match
        // Implementation for encrypting a message
            m_Pairing_aes_encrypt.AES_CBC_encrypt_buffer(packet.GetBodyDataAsStr(),packet.GetPacketDataAsBytes());

        }
        return true;
    }

    Msg * TCPConnection::DecryptMsg(MsgPacket& packet)
    {
        if(m_Pairing == false)
        {
            const IVToKeyMap& ivToKeyMap =  AESAccessManagement::Get()->GetIVToKeyMap();
            cout << "Decrypting NON PAIRING Msg " + packet.GetName() + " with : " << endl;
            m_aes_encrypt.PrintKeyAndIV();

            if(EncryptionSetupComplete == true )
            {
                std::vector<uint8_t> decryptedData = m_aes_decrypt.AES_CBC_decrypt_buffer(packet.GetPacketDataAsBytes());
                if(decryptedData.size() > 0)
                {
                    packet.SetBodyDataFromBytes(decryptedData);
                    Msg* pMsg = MsgManager::GetInstance().CreateMsgFromPacket(packet);
                    if(pMsg != nullptr)
                    {
                        return pMsg;
                    }
                }
            }
            else
            {
                for(const auto& ivKeyPair : ivToKeyMap)
                {
                    CryptoBlockVector serverIV = ivKeyPair.first;
                    CryptoBlockVector serverKey = ivKeyPair.second;

                    m_aes_decrypt.AES_init_ctx_iv(serverKey, serverIV);
                    m_aes_encrypt.AES_init_ctx_iv(serverKey, serverIV);

                    cout << "TRYING Decrypting  PAIRING Msg " + packet.GetName() + " with : " << endl;
                    m_aes_decrypt.PrintKeyAndIV();

                    std::vector<uint8_t> decryptedData = m_aes_decrypt.AES_CBC_decrypt_buffer(packet.GetPacketDataAsBytes());
                    if(decryptedData.size() > 0)
                    {
                        packet.SetBodyDataFromBytes(decryptedData);
                        Msg* pMsg = MsgManager::GetInstance().CreateMsgFromPacket(packet);
                        if(pMsg != nullptr)
                        {
                            EncryptionSetupComplete = true; // Set flag to true after successful decryption with any key-IV pair
                            return pMsg;
                        }
                    }
                }

            }
            return nullptr;   
        }
        else
        {

            std::vector<uint8_t> decryptedData = m_Pairing_aes_decrypt.AES_CBC_decrypt_buffer(packet.GetPacketDataAsBytes());
            if(decryptedData.size() > 0)
            {
                packet.SetBodyDataFromBytes(decryptedData);
                Msg* pMsg = MsgManager::GetInstance().CreateMsgFromPacket(packet);
                return pMsg;
            }
        }
        return nullptr;
    }
} // namespace CE::tcp

