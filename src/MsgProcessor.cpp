#include <MsgProcessor.h>

#include <Msg.h>
#include <SystemMsgs.h>

namespace CE::tcp
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

    void MsgProcessor::RemoveMsgCreator(ICreateMsgFromPacket* msgCreator)
    {
        auto it = std::find(m_MsgCreators.begin(), m_MsgCreators.end(), msgCreator);
        if (it != m_MsgCreators.end()) {
            m_MsgCreators.erase(it);
        }
    }

    void MsgProcessor::RemoveMsgProcessor(IMsgProcessor* msgProcessor)
    {
        auto it = std::find(m_MsgProcessors.begin(), m_MsgProcessors.end(), msgProcessor);
        if (it != m_MsgProcessors.end()) {
            m_MsgProcessors.erase(it);
        }
    }

    bool MsgProcessor::ProcessMsg(Msg* pMsg)
    {
        bool completed = false;
        for(size_t i = 0; i < m_MsgProcessors.size(); ++i)
        {
            completed |= m_MsgProcessors[i]->ProcessMsg(*pMsg);
        }   
        return completed;
    }
    bool MsgProcessor::ProcessMsgFromPacket(MsgPacket& packet)
    {
        bool completed = false;
        Msg* msg  = CreateMsgFromPacket(packet);
        if(msg != nullptr)        
        {

            for(size_t i = 0; i < m_MsgProcessors.size(); ++i)
            {
                completed |= m_MsgProcessors[i]->ProcessMsg(*msg);
            }   
            delete msg;
        }
        return completed;
    }

    Msg* MsgProcessor::CreateMsgFromPacket(MsgPacket& packet)
    {
        for(size_t i = 0; i < m_MsgCreators.size(); ++i)
        {
            Msg* pMsg = m_MsgCreators[i]->CreateMsg(packet);
            if(pMsg != nullptr)
            {
                return pMsg;
            }
        }   
        return nullptr;
    }
} // namespace CE::tcp

