#include <MsgPacket.h>
#include <sys/socket.h>
#include <unistd.h>
#include <SystemMsgs.h>
#include <MsgManager.h>
#include <string_view>
#include <cstdint>
namespace CE::tcp
{
    MsgPacket::MsgPacket(std::string name) : m_ConnectionID(-1)
    {
        // Initialize packet data with default values
        m_Data.Prefix = 0xDEADBEEF; // Example prefix
        m_Data.MsgID = MsgManager::GetInstance().GetMsgId(name);
        m_Data.MsgBodySize = 0;
        m_Data.IsEncrypted = false;
        m_Data.dummy1 = 0;
        m_Data.dummy2 = 0;
        m_Data.dummy3 = 0;
        m_Data.Postfix = 0xBEEFDEAD; // Example postfix
        ServerType = LOCAL_SERVER;

    }
    

    MsgPacket::~MsgPacket()
    {
        // Destructor implementation
    }
    int MsgPacket::GetPrefix() const
    {
        return m_Data.Prefix;
    }
    void MsgPacket::SetMsgID(int msgID)
    {
        m_Data.MsgID = msgID;
    }
    int MsgPacket::GetMsgID() const
    {
        return m_Data.MsgID;
    }           

    void MsgPacket::SetServerType(TCPServerTypes type)
    {
        ServerType = type;
    }
    TCPServerTypes MsgPacket::GetServerType () const
    {
        return ServerType;
    }

    bool MsgPacket::GetIsEncrypted() const
    {
        return m_Data.IsEncrypted;
    }
    void MsgPacket::SetIsEncrypted(bool isEncrypted)
    {
        m_Data.IsEncrypted = isEncrypted;
    }


    int MsgPacket::GetPostfix() const
    {
        return m_Data.Postfix;
    }   
} // namespace CE::tcp


