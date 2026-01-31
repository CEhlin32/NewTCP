#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <PublisherBase.h>
#include <AESEncryptor.h>
#include <AESDecryptor.h>
#include <Thread.h>
#include <Msg.h>
#include <vector>

namespace newtcp
{
    class TCPConnection : public PublisherBase
    {
    public:

        TCPConnection(int socket_fd);
        ~TCPConnection();

        int Send(Msg& msg);
        static int SendToAll(Msg& msg);
        
        bool IsEncryptionEnabled() const;  
        int GetSocketFd() const;
        
        void EnableEncryption(const CryptoBlockVector &iv, const CryptoBlockVector &key);
        bool EncryptMsg(MsgPacket& packet);
        bool DecryptMsg(MsgPacket& packet);
    protected:
        void ReadThreadFunction();    
    private:
        int m_socket_fd;
        bool m_EncryptionEnabled;
        AESEncryptor m_aes_encrypt;
        AESDecryptor m_aes_decrypt;

        NamedThread *m_pReadThread;
        thread::id m_ReadThreadId;
        bool m_KeepAlive;
        static std::vector<TCPConnection*> m_ActiveConnections;

    };
} // namespace newtcp



#endif // TCP_CONNECTION_H