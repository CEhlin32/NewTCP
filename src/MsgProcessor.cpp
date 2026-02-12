#include <MsgProcessor.h>
#include <MsgCmds.h>
#include <Msg.h>
#include <SystemMsgs.h>

namespace newtcp
{
    std::vector<IMsgProcessor*> MsgProcessor::m_MsgProcessors;

    MsgProcessor::MsgProcessor()
    {
    }

    MsgProcessor::~MsgProcessor()
    {
    }
    void MsgProcessor::AddMsgProcessor(IMsgProcessor* msgProcessor)
    {
        m_MsgProcessors.push_back(msgProcessor);
    }
    bool MsgProcessor::ProcessMsgPacket(MsgPacket& packet)
    {
        bool completed = false;
        for(int i = 0; i < m_MsgProcessors.size(); ++i)
        {
            completed |= m_MsgProcessors[i]->ProcessMsgFromPacket(packet);
        }   
        return completed;
    }

    Msg* MsgProcessor::CreateMsgFromPacket(MsgPacket& packet)
    {
        for(int i = 0; i < m_MsgProcessors.size(); ++i)
        {
            Msg* pMsg = m_MsgProcessors[i]->CreateMsgFromPacketInternal(packet);
            if(pMsg != nullptr)
            {
                return pMsg;
            }
        }   
        return nullptr;
    }
} // namespace newtcp

