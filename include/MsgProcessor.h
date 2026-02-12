#ifndef NEW_MSG_FACTORY_H
#define NEW_MSG_FACTORY_H

#include <vector>
#include <Msg.h>
#include <MsgPacket.h>


namespace newtcp
{

    class IMsgProcessor
    {
    public:
        virtual bool ProcessMsgFromPacket(MsgPacket& packet) = 0;
        virtual Msg* CreateMsgFromPacketInternal(MsgPacket& packet) = 0;
    };
    
    class MsgProcessor
    {
    public:
        MsgProcessor();
        ~MsgProcessor();
        static void AddMsgProcessor (IMsgProcessor* msgProcessor);
        static bool ProcessMsgPacket(MsgPacket& packet);
        static Msg* CreateMsgFromPacket(MsgPacket& packet);
protected:
        virtual bool ProcessMsgFromPacket(MsgPacket& packet) = 0;
        virtual Msg* CreateMsgFromPacketInternal(MsgPacket& packet) = 0;
        private:
        static std::vector<IMsgProcessor*> m_MsgProcessors;
    };

} // namespace newtcp



#endif // NEW_MSG_FACTORY_H