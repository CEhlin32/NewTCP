#include "Msg.h"
#include <MsgPacket.h>

namespace CE::tcp
{
    Msg::Msg(std::string name, int msgID) : m_Name(name), m_MsgPacket(name, msgID)
    {
        // Constructor implementation
    }

    Msg::Msg(std::string name, MsgPacket& packet) : m_Name(name), m_MsgPacket(packet)
    {
        // Copy constructor implementation
        DeSerializeBody();
    }

    Msg::~Msg()
    {
        // Destructor implementation

    }

    MsgPacket& Msg::Serialize() 
    {
        // Serialize body first
        SerializeBody();
        // move bodyData to MsgPacket if not encrypting
        m_MsgPacket.SetPacketDataFromStr( m_MsgPacket.GetBodyDataAsStr());
        return m_MsgPacket;
    }

    std::string Msg::GetName() const
    {
        return m_Name;
    }

    MsgPacket& Msg::GetMsgPacket()
    {
        return m_MsgPacket;
    }
    
    int Msg::GetMsgID() const
    {
        return m_MsgPacket.GetMsgID();
    }
    int Msg::GetConnectionID() const
    {
        return m_MsgPacket.GetConnectionID();
    }

} // namespace CE::tcp
