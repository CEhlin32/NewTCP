#ifdef OLD_CODE

#include <Msg.h>
#include <SystemMsgs.h>

namespace CE::tcp
{
    // Static registry definitions — shared across all MsgProcessor instances
    std::vector<IMsgProcessor*> MsgProcessor::m_MsgProcessors;
    std::vector<ICreateMsgFromPacket*> MsgProcessor::m_MsgCreators;

    MsgProcessor::MsgProcessor()
    {
    }

    MsgProcessor::~MsgProcessor()
    {
    }

    // Append the creator to the registry so future packet-to-Msg conversions
    // will query it
    void MsgProcessor::AddMsgCreator(ICreateMsgFromPacket* msgCreator)
    {
        m_MsgCreators.push_back(msgCreator);
    }

    // Append the processor to the registry so future message dispatches
    // will invoke it
    void MsgProcessor::AddMsgProcessor(IMsgProcessor* msgProcessor)
    {
        m_MsgProcessors.push_back(msgProcessor);
    }

    // Find the creator by pointer and remove the first matching entry;
    // if the pointer is not registered this is a no-op
    void MsgProcessor::RemoveMsgCreator(ICreateMsgFromPacket* msgCreator)
    {
        auto it = std::find(m_MsgCreators.begin(), m_MsgCreators.end(), msgCreator);
        if (it != m_MsgCreators.end()) {
            m_MsgCreators.erase(it);
        }
    }

    // Find the processor by pointer and remove the first matching entry;
    // if the pointer is not registered this is a no-op
    void MsgProcessor::RemoveMsgProcessor(IMsgProcessor* msgProcessor)
    {
        auto it = std::find(m_MsgProcessors.begin(), m_MsgProcessors.end(), msgProcessor);
        if (it != m_MsgProcessors.end()) {
            m_MsgProcessors.erase(it);
        }
    }

    // Dispatch a pre-built Msg to every registered processor.
    // All processors are always visited (no short-circuit); completed is true
    // if at least one processor reported it handled the message.
    bool MsgProcessor::ProcessMsg(Msg* pMsg)
    {
        cout << "Processing Msg " + pMsg->GetName()  << endl;
        bool completed = false;
        for(size_t i = 0; i < m_MsgProcessors.size(); ++i)
        {
            completed |= m_MsgProcessors[i]->ProcessMsg(*pMsg);
        }   
        return completed;
    }

    // Two-phase dispatch: first build a typed Msg from the raw packet, then
    // route it through all registered processors.
    bool MsgProcessor::ProcessMsgFromPacket(MsgPacket& packet)
    {
        bool completed = false;

        // Phase 1 — ask registered creators to deserialise the packet into a Msg
        Msg* msg  = CreateMsgFromPacket(packet);
        if(msg != nullptr)        
        {
            // Phase 2 — offer the Msg to every registered processor;
            // accumulate whether any of them handled it
            for(size_t i = 0; i < m_MsgProcessors.size(); ++i)
            {
                completed |= m_MsgProcessors[i]->ProcessMsg(*msg);
            }   

            // Processors do not own the Msg — release it here
            delete msg;
        }
        return completed;
    }

    // Walk the creator registry in registration order; return the first
    // successfully created Msg.  Returns nullptr if no creator recognises
    // the packet, which causes ProcessMsgFromPacket() to skip dispatch.
    Msg* MsgProcessor::CreateMsgFromPacket(MsgPacket& packet)
    {
        for(size_t i = 0; i < m_MsgCreators.size(); ++i)
        {
            Msg* pMsg = m_MsgCreators[i]->CreateMsg(packet);
            if(pMsg != nullptr)
            {
                // First creator that claims the packet wins — stop here
                return pMsg;
            }
        }   
        return nullptr;
    }
} // namespace CE::tcp
#endif // OLD_CODE
