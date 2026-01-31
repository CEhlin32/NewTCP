#ifndef NEW_MSG_FACTORY_H
#define NEW_MSG_FACTORY_H

#include <vector>
#include <Msg.h>
#include <MsgPacket.h>


class NewMsgFactory
{
public:
    NewMsgFactory();
    ~NewMsgFactory();
    static void AddMessageFactory(NewMsgFactory* factory);
    static Msg* CreateMessage(MsgPacket& packet);
    virtual Msg* CreateMsgInstance(MsgPacket& packet) = 0;

    private:
    static std::vector<NewMsgFactory*> m_Factories;
};

class SystemMsgFactory : public NewMsgFactory
{
public:
    SystemMsgFactory();
    ~SystemMsgFactory();    
    virtual Msg* CreateMsgInstance(MsgPacket& packet) override;
};

#endif // NEW_MSG_FACTORY_H