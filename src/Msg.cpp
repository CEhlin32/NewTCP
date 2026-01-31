#include "Msg.h"
#include <MsgPacket.h>

namespace newtcp
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

    MsgPacket& Msg::Serialize(bool encrypt /*= false*/) 
    {
        m_MsgPacket.SetIsEncrypted(encrypt);
        // Serialize body first
        SerializeBody();
        // move bodyData to MsgPacket if not encrypting
        if(false == encrypt)
            m_MsgPacket.SetBodyDataFromStr( m_MsgPacket.GetBodyDataAsStr());

        return m_MsgPacket;
    }

    std::string Msg::GetName() const
    {
        return m_Name;
    }
} // namespace newtcp
