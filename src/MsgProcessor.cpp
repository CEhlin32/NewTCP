#include <MsgProcessor.h>
#include <MsgCmds.h>
#include <Msg.h>
#include <SystemMsgs.h>

namespace newtcp
{
    std::vector<IMsgProcessor*> MsgProcessor::m_MsgProcessors;
    std::vector<ICreateMsgFromPacket*> MsgProcessor::m_MsgCreators;

    MsgProcessor::MsgProcessor()
    {
    }

    MsgProcessor::~MsgProcessor()
    {
    }

    void MsgProcessor::AddMsgCreator(ICreateMsgFromPacket* msgCreator)
    {
        m_MsgCreators.push_back(msgCreator);
    }

    void MsgProcessor::AddMsgProcessor(IMsgProcessor* msgProcessor)
    {
        m_MsgProcessors.push_back(msgProcessor);
    }
    bool MsgProcessor::ProcessMsgFromPacket(MsgPacket& packet)
    {
        bool completed = false;
        Msg* msg  = CreateMsgFromPacket(packet);
        if(msg != nullptr)        
        {

            for(int i = 0; i < m_MsgProcessors.size(); ++i)
            {
                completed |= m_MsgProcessors[i]->ProcessMsg(*msg);
            }   
            delete msg;
        }
        return completed;
    }

    Msg* MsgProcessor::CreateMsgFromPacket(MsgPacket& packet)
    {
        for(int i = 0; i < m_MsgCreators.size(); ++i)
        {
            Msg* pMsg = m_MsgCreators[i]->CreateMsgFromPacketInternal(packet);
            if(pMsg != nullptr)
            {
                return pMsg;
            }
        }   
        return nullptr;
    }
} // namespace newtcp

