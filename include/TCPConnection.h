#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <MsgProcessor.h>
#include <AESEncryptor.h>
#include <AESDecryptor.h>
#include <Thread.h>
#include <Msg.h>
#include <vector>
#include <PublisherBase.h>
#include <EnumMsgIDMgr.h>
namespace CE::tcp
{

    class TCPConnectionStatus
    {
    public:
        enum ServerStatus
        {
            Connection_Available,
            Connection_Connected,
            Connection_ShuttingDown,
            Connection_StartingUp
        };

        TCPConnectionStatus() : status(Connection_Available) 
        {  
            
        }

        ServerStatus status;

        inline static const std::vector<std::string> ServerStatusStr;
    };

    class ConnectionCompleteData : public PublishDataBase
    {
    public:
        ConnectionCompleteData(int connectionID) : PublishDataBase("ConnectionCompleteData")
        {
            ConnectionID = connectionID;
        }
        int ConnectionID;   
    };

    class TCPConnection : public IMsgProcessor
    {
    public:

        TCPConnection();
        virtual ~TCPConnection();
        static std::vector<TCPConnection*> m_ActiveConnections;

        bool StartReadThread();
        void StopReadThread();
        int Send(Msg& msg);
        static int SendToAll(Msg& msg);

        int GetSocketFd() const;
        void SetSocketFd(int socketFd);  
        void EnableEncryption(const CryptoBlockVector &iv, const CryptoBlockVector &key);
        bool EncryptMsg(MsgPacket& packet);
        Msg* DecryptMsg(MsgPacket& packet);
        TCPConnectionStatus::ServerStatus GetConnectionStatus() const;
        static int SubscribeToConnectionComplete(SubscriberBase *subscriber);
    protected:
        void ReadThreadFunction();   
        bool ProcessMsg(Msg& msg) override; 
        NamedThread *m_pReadThread;
        thread::id m_ReadThreadId;
        TCPConnectionStatus m_ConnectionStatus;

        private:
        bool m_Pairing;
        bool EncryptionSetupComplete;
        int m_socket_fd;
        AESEncryptor m_aes_encrypt;
        AESDecryptor m_aes_decrypt;

        AESEncryptor m_Pairing_aes_encrypt;
        AESDecryptor m_Pairing_aes_decrypt;

        std::mutex m_SendMutex; // Mutex for synchronizing access to the send function

        bool m_KeepAlive;
        static PublisherBase m_Publisher;

    };
} // namespace CE::tcp



#endif // TCP_CONNECTION_H