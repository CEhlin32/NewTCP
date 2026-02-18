#ifndef NEW_MSG_FACTORY_H
#define NEW_MSG_FACTORY_H

#include <vector>
#include <Msg.h>
#include <MsgPacket.h>


namespace CE::tcp
{

    class IMsgProcessor
    {
    public:
        virtual bool ProcessMsg(Msg& msg) = 0;
    };

    class ICreateMsgFromPacket
    {
    public:
        virtual Msg* CreateMsgFromPacketInternal(MsgPacket& packet) = 0;
    };

    class MsgProcessor
    {
    public:
        MsgProcessor();
        ~MsgProcessor();
        static void AddMsgProcessor (IMsgProcessor* msgProcessor);
        static void AddMsgCreator (ICreateMsgFromPacket* msgCreator);
        static bool ProcessMsgFromPacket(MsgPacket& packet);
        static Msg* CreateMsgFromPacket(MsgPacket& packet);
protected:
        virtual bool ProcessMsg(Msg& msg) = 0;
        virtual Msg* CreateMsgFromPacketInternal(MsgPacket& packet) = 0;
        private:
        static std::vector<IMsgProcessor*> m_MsgProcessors;
        static std::vector<ICreateMsgFromPacket*> m_MsgCreators;

    };

} // namespace CE::tcp



#endif // NEW_MSG_FACTORY_H