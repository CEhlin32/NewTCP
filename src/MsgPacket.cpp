#include <MsgPacket.h>
#include <sys/socket.h>
#include <unistd.h>



MsgPacket::MsgPacket(std::string name, int msgID) : m_Name(name)
{
    // Initialize packet data with default values
    m_Data.Prefix = 0xDEADBEEF; // Example prefix
    m_Data.MsgID = msgID;
    m_Data.MsgBodySize = 0;
    m_Data.PacketHandlerType = 0;
    m_Data.IsEncrypted = false;
    m_Data.Postfix = 0xBEEFDEAD; // Example postfix
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
void MsgPacket::SetPacketHandlerType(int type)
{
    m_Data.PacketHandlerType = type;
}
int MsgPacket::GetPacketHandlerType() const
{
    return m_Data.PacketHandlerType;
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

