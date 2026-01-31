#include <TCPConnection.h>    
#include <unistd.h>
#include <sys/socket.h>
#include <algorithm>
#include <MsgPacket.h>
#include <MyExceptions.h>
#include <NewMsgFactory.h>



std::vector<TCPConnection*> TCPConnection::m_ActiveConnections;

TCPConnection::TCPConnection(int socket_fd) : PublisherBase("TCPConnection"),
    m_socket_fd(socket_fd), 
    m_EncryptionEnabled(false),
    m_pReadThread(nullptr),
    m_KeepAlive(true)
{
    m_pReadThread = new NamedThread("TCPConnectionRead", &TCPConnection::ReadThreadFunction, this);
    m_ActiveConnections.push_back(this);
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

void TCPConnection::ReadThreadFunction()
{
    // Implementation for reading data from the socket
    while(m_KeepAlive == true)
    {
        char buffer[1024];
        // Read Msg header
        MsgPacket packet;
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

            Msg* pMsg = NewMsgFactory::CreateMessage(packet);

        }

        // Publish msg to subscribers
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
        /* code */
        MsgPacket& packet = msg.Serialize(m_EncryptionEnabled);

        if(m_EncryptionEnabled)
        {
            // Encrypt msg here
            EncryptMsg(packet);
        }
        
        if(packet.GetMsgHdrSize() != send(m_socket_fd, packet.GetMsgHdrDataPtr(),packet.GetMsgHdrSize(), 0))
        {
            return -2; // Send failed
        }
        if(packet.GetPacketDataAsBytes().size() > 0)
        {
            // Send Msg body here
            if(packet.GetPacketDataAsStr().size() != 
                send(m_socket_fd, packet.GetPacketDataAsStr().c_str(), packet.GetPacketDataAsStr().size(), 0))
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
//    CreatePacketFromBodyData(packet.GetMsgID());
    return true;
}
