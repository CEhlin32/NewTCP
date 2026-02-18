#ifndef MSG_H
#define MSG_H

#include <string>
#include <MsgPacket.h>

namespace CE::tcp
{
    class Msg
    {
    public:
        Msg(std::string name, int msgID = -1);
        Msg(std::string name, MsgPacket& packet);
        ~Msg();
        MsgPacket& Serialize();
        std::string GetName() const;
        int GetConnectionID() const;
        int GetMsgID() const;
        MsgPacket& GetMsgPacket(); 
 
        protected:
        virtual void SerializeBody(){/*Default is no Body to Serialize*/;}
        virtual void DeSerializeBody() {/*Default is no Body to Deserialize*/;}

        // Add members and methods as needed
        std::string m_Name;
        MsgPacket m_MsgPacket;
    };
} // namespace CE::tcp



#endif // MSG_H