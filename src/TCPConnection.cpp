#include <TCPConnection.h>    
#include <unistd.h>
#include <sys/socket.h>
#include <algorithm>
#include <MsgPacket.h>
#include <MyExceptions.h>
#include <MsgProcessor.h>

#include <SystemMsgs.h>
#include <Autherization.h>
#include <Logger.h>
#include <DebugSupport.h>
#include <EnumExtender.h>
#include <EnumMsgIDMgr.h>

#include <SystemMsgConstants.cs.h>

#ifdef OLD_CODE
#include <TCPMsgs.h>
#endif
namespace CE::tcp
{
    std::vector<TCPConnection*> TCPConnection::m_ActiveConnections;
    PublisherBase TCPConnection::m_Publisher;

    TCPConnection::TCPConnection() :
        m_socket_fd(-1), 
        m_pReadThread(nullptr),
        m_KeepAlive(true),
        EncryptionSetupComplete(false),
        m_Pairing(false)
    {
        m_ConnectionStatus.status = TCPConnectionStatus::Connection_Available;
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
        MsgProcessor::AddMsgProcessor(this);
        MsgManager::Get().AddToMsgProcessors("TCPSystemCommands", this);
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
        MsgProcessor::RemoveMsgProcessor(this);
#ifdef OLD_CODE        
        MsgProcessor::RemoveMsgCreator(this);

        //ce_sprinkler::MsgManager::Get().RemoveMsgProcessor(this);
#endif        
    }   

    TCPConnectionStatus::ServerStatus TCPConnection::GetConnectionStatus() const
    {
        return m_ConnectionStatus.status;
    }
    
    void TCPConnection::ReadThreadFunction()
    {
        m_ActiveConnections.push_back(this);
#ifdef OLD_CODE        
        // Send AvailableCmdInfoMsg to client
        AvailableCmdInfoMsg cmdInfoMsg;
        Send(cmdInfoMsg);
#endif
        m_KeepAlive = true;
        m_Pairing = false; // Reset pairing mode when starting read thread for a new connection
        EncryptionSetupComplete = false;
        // Implementation for reading data from the socket
        while(m_KeepAlive == true)
        {
            char buffer[1024];
            // Read Msg header
            MsgPacket packet;
            packet.SetConnectionID(m_socket_fd); // Set connection ID if needed
            int numBytesRead = read(m_socket_fd, packet.GetMsgHdrDataPtr(),packet.GetMsgHdrSize());
                if(packet.GetMsgHdrSize() != numBytesRead)
            {
                // Handle read error or disconnection
                break;
            }
        
            // Read Msg body using size from header
            int sizeToRead = packet.GetMsgBodySize();

            if(sizeToRead > 0)
            {
                // If encryption is enabled, we need to read the data in multiples of the AES block size (16 bytes)
                if(packet.GetIsEncrypted())
                {
                    sizeToRead = sizeToRead + (16 - (sizeToRead % 16)); 
//                    sizeToRead = ((sizeToRead + 15) / 16) * 16; // AES block size is 16 bytes
                }
                // Create a buffer to hold the received data
                std::vector<uint8_t> receivedData(sizeToRead);
                // Read the data into the buffer
                int sizeRead = read(m_socket_fd, receivedData.data(), sizeToRead);
                // 
                if(sizeToRead != sizeRead)
                {
                    // Handle read error or disconnection
                    break;  
                }

                packet.SetPacketDataFromBytes(receivedData);
                if(packet.GetMsgID() == SharedSysMsgConstants::AutherizationStartRequestCmd)
                {
                    // Handle pairing mode for AutherizationStartRequestCmd without decryption  
                    m_Pairing = true;
                    packet.SetBodyDataFromStr(packet.GetPacketDataAsStr());
                    Msg* pMsg = new AutherizationStartRequestMsg(packet);
                    bool msgProcessed = MsgProcessor::ProcessMsg(pMsg);
                    delete pMsg;
                    continue;
                }
                else
                {
                    // Decrypt Msg here
                    Msg *pMsg = DecryptMsg(packet);
                    printf("*******************\n Finished decrypting Msg %s\n\n", MsgManager::Get().GetMsgNameFromID(packet.GetMsgID()).c_str());
                    if( pMsg == nullptr)
                    {
                        if(packet.GetMsgID() == SharedSysMsgConstants::ValidateIVCmd)
                        {
                            // Send ConnectionRefusedMsg with reason code for invalid IV 
                            // if decryption fails for ValidateIVCmd, which is critical for security,
                            // new Pairing is required.
                            ConnectionRefusedMsg resultMsg;
                            resultMsg.SetReasonCode(ConnectionRefusedMsg::InvalidIV); // Set reason code for refusal
                            Send(resultMsg);
                            continue;
                        }
                        else
                        {
                            ConnectionRefusedMsg resultMsg;
                            resultMsg.SetReasonCode(ConnectionRefusedMsg::DecryptionFailed); // Set reason code for refusal
                            Send(resultMsg);
                            continue;
                        }
                        break;
                    }
                    else
                    {
                        bool msgProcessed = MsgProcessor::ProcessMsg(pMsg);
                        delete pMsg;
                    }
                }

            }
            else
            {
                // If no body data, we can process the Msg directly
                packet.SetBodyDataFromBytes(packet.GetPacketDataAsBytes());
                bool msgProcessed = MsgProcessor::ProcessMsgFromPacket(packet);
            }

        }

#ifdef OLD_CODE        
        MsgProcessor::RemoveMsgCreator(this);
#endif        

        m_KeepAlive = false;
            close(m_socket_fd);
        m_ActiveConnections.erase(std::remove(m_ActiveConnections.begin(), m_ActiveConnections.end(), this), m_ActiveConnections.end());
        
        MsgProcessor::RemoveMsgProcessor(this);
    }

#ifdef OLD_CODE    
    Msg* TCPConnection::CreateMsg(MsgPacket& packet)
    {
        EnumExtenderManager& enumManager = TCPMsgEnumManager::Get();
        int relID = enumManager.GetRelativeID(packet.GetMsgID(), "TCPSystemCommands");
        switch(relID)
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
    #endif

    bool TCPConnection::ProcessMsg(Msg& msg)
    {
        int absMessageID = msg.GetMsgID();
        int relMessageID = TCPMsgEnumManager::Get().GetRelativeID(absMessageID, "TCPSystemCommands");
        switch(relMessageID)
        {
            case SharedSysMsgConstants::EnableEncryptDecryptCmd:
            {               
                // send msg info data
                break;
            }
            case SharedSysMsgConstants::AutherizationStartRequestCmd:
            {
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

            case SharedSysMsgConstants::AutherizationValidationCmd:
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
                    IVAndKeyValues keyAndIV(Autherization::Get()->GetKey(), ivToValidate);

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

            case SharedSysMsgConstants::RequestKeyAndIVCmd:
            {
                UpdateKeyAndIVMsg setKeyAndIVMsg;
                AESAccessManagement::Get()->AddASEKeyAndIV(setKeyAndIVMsg.GetIVAndKeyValues());
                Send(setKeyAndIVMsg);

            }
            break;

            case SharedSysMsgConstants::ValidateIVCmd:
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
            case SharedSysMsgConstants::DebugQueryCmd:
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

            lock_guard<std::mutex> lock(m_SendMutex); // Ensure thread safety for sending messages
            /* code */
            MsgPacket& packet = msg.Serialize();
            packet.SetConnectionID(m_socket_fd); // Set connection ID if needed

            if(packet.GetIsEncrypted())
            {
                // Encrypt msg here
                EncryptMsg(packet);
            }
            if( -1 == packet.GetMsgID())
            {
                TheAppLogger.LogMsgWithTime(DebugErrorLogOption::instance(),
                    "Failed to serialize message %s for sending on connection ID = %d\n", 
                    msg.GetName().c_str(), m_socket_fd);
                return -1; // Serialization failed
            }
            printf("*******************/nSending Msg: Id = %d, %s with Encryption = %d\n", packet.GetMsgID(), MsgManager::Get().GetMsgNameFromID(packet.GetMsgID()).c_str(), packet.GetIsEncrypted());   ;
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
            printf("*******************/n Finsished sendingMsg  %s\n\n", MsgManager::Get().GetMsgNameFromID(packet.GetMsgID()).c_str());   
        }
        catch(const MyExceptions::IOException& e)
        {
            return -1; // Exception occurred
        }
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
            // During pairing mode, we use the IV and key set for this connection without trying to find a match
            printf("*******************\n Encrypting Msg %s in NOT IN pairing mode\n\t IV =", MsgManager::Get().GetMsgNameFromID(packet.GetMsgID()).c_str());
            for (int i = 0; i < 16; ++i) 
            {
                printf("%u ", m_aes_encrypt.GetIV()[i]);
            }
            printf("\n**********\n\n");
            m_aes_encrypt.AES_CBC_encrypt_buffer(packet.GetBodyDataAsStr(),packet.GetPacketDataAsBytes());

        }
        else
        {
            // During pairing mode, we use the IV and key set for this connection without trying to find a match
            printf("*******************\n Encrypting Msg %s in pairing mode\n\t IV =", MsgManager::Get().GetMsgNameFromID(packet.GetMsgID()).c_str());
            for (int i = 0; i < 16; ++i) 
            {
                printf("%u ", m_Pairing_aes_encrypt.GetIV()[i]);
            }
            printf("\n**********\n\n");
        // Implementation for encrypting a message
            m_Pairing_aes_encrypt.AES_CBC_encrypt_buffer(packet.GetBodyDataAsStr(),packet.GetPacketDataAsBytes());

        }
        printf("*******************\n Finished encrypting Msg %s\n\n", MsgManager::Get().GetMsgNameFromID(packet.GetMsgID()).c_str());
        return true;
    }

    Msg * TCPConnection::DecryptMsg(MsgPacket& packet)
    {
        if(m_Pairing == false)
        {
            int id = packet.GetMsgID();

            const IVToKeyMap ivToKeyMap =  AESAccessManagement::Get()->GetIVToKeyMap();
            if(EncryptionSetupComplete == true )
            {
                std::vector<uint8_t> decryptedData = m_aes_decrypt.AES_CBC_decrypt_buffer(packet.GetPacketDataAsBytes());
                if(decryptedData.size() > 0)
                {
                    packet.SetBodyDataFromBytes(decryptedData);
                    Msg* pMsg = MsgProcessor::CreateMsgFromPacket(packet);
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


                    printf("*******************\n Attempting to decrypt Msg %s NOT in pairing mode\n\t IV =", MsgManager::Get().GetMsgNameFromID(packet.GetMsgID()).c_str());
                    for (int i = 0; i < 16; ++i) 
                        {
                            printf("%u ", serverIV[i]);
                    }
                    printf("\n**********\n\n");


                    m_aes_decrypt.AES_init_ctx_iv(serverKey, serverIV);
                    m_aes_encrypt.AES_init_ctx_iv(serverKey, serverIV);
                    std::vector<uint8_t> decryptedData = m_aes_decrypt.AES_CBC_decrypt_buffer(packet.GetPacketDataAsBytes());
                    if(decryptedData.size() > 0)
                    {
                        packet.SetBodyDataFromBytes(decryptedData);
                        Msg* pMsg = MsgProcessor::CreateMsgFromPacket(packet);
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
            printf("*******************\n Attempting to decrypt Msg %s in pairing mode\n\t IV =", MsgManager::Get().GetMsgNameFromID(packet.GetMsgID()).c_str());
            for (int i = 0; i < 16; ++i) 
            {
                printf("%u ", m_Pairing_aes_decrypt.GetIV()[i]);
            }
            printf("\n**********\n\n");
            std::vector<uint8_t> decryptedData = m_Pairing_aes_decrypt.AES_CBC_decrypt_buffer(packet.GetPacketDataAsBytes());
            if(decryptedData.size() > 0)
            {
                packet.SetBodyDataFromBytes(decryptedData);
                Msg* pMsg = MsgProcessor::CreateMsgFromPacket(packet);
                return pMsg;
            }
        }
        return nullptr;
    }
} // namespace CE::tcp

