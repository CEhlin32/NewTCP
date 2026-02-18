#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <MsgProcessor.h>
#include <AESEncryptor.h>
#include <AESDecryptor.h>
#include <Thread.h>
#include <Msg.h>
#include <vector>

namespace newtcp
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



    class TCPConnection : public IMsgProcessor, public ICreateMsgFromPacket
    {
    public:

        TCPConnection();
        ~TCPConnection();
        static std::vector<TCPConnection*> m_ActiveConnections;

        bool StartReadThread();
        void StopReadThread();
        int Send(Msg& msg);
        static int SendToAll(Msg& msg);
        
        bool IsEncryptionEnabled() const;  
        int GetSocketFd() const;
        void SetSocketFd(int socketFd);  
        void EnableEncryption(const CryptoBlockVector &iv, const CryptoBlockVector &key);
        bool EncryptMsg(MsgPacket& packet);
        bool DecryptMsg(MsgPacket& packet);
        TCPConnectionStatus::ServerStatus GetConnectionStatus() const;

    protected:
        void ReadThreadFunction();   
        bool ProcessMsg(Msg& msg) override; 
        Msg* CreateMsgFromPacketInternal(MsgPacket& packet) override;
        NamedThread *m_pReadThread;
        thread::id m_ReadThreadId;
        TCPConnectionStatus m_ConnectionStatus;

        private:
        int m_socket_fd;
        bool m_EncryptionEnabled;
        AESEncryptor m_aes_encrypt;
        AESDecryptor m_aes_decrypt;

        bool m_KeepAlive;

    };
} // namespace newtcp



#endif // TCP_CONNECTION_H