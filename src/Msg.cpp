#include "Msg.h"
#include <MsgPacket.h>

namespace CE::tcp
{
    Msg::Msg(std::string name, int msgID) : m_Name(name), m_MsgPacket(name, msgID)
    {
        m_MsgPacket.SetIsEncrypted(true); // Default to encrypted, can be overridden by derived classes;
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
        m_MsgPacket.SetMsgBodySize( m_MsgPacket.GetBodyDataAsStr().size());

        if(m_MsgPacket.GetBodyDataAsStr().size() == 0)
        {
            m_MsgPacket.SetIsEncrypted(false); // Mark packet as not encrypted if body size is 0, can be overridden by derived classes
        }
        else
        {
            m_MsgPacket.SetIsEncrypted(true); // Mark packet as encrypted if body size is greater than 0, can be overridden by derived classes
        }
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
