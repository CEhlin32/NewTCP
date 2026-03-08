#include <EnumMsgIDMgr.h>

namespace Test
{
        int EnumMsgIDMgr::AddEnumIDs(const EnumIDs& enumIDs)
        {
            int numOfIDs = enumIDs.GetEnumValueNames().size();
            int baseIndex = GetNextID(numOfIDs);
            AssignedEnumMsgIDs assignedIDs(enumIDs, baseIndex);
            m_AssignedEnumMsgIDs.push_back(assignedIDs);
            return baseIndex;

        }
        AssignedEnumMsgIDs EnumMsgIDMgr::GetEnumIDsFor(int absID) const
        {
            for(const auto& assignedIDs : m_AssignedEnumMsgIDs)
            {
                if(absID >= assignedIDs.GetBaseIndex() && absID < assignedIDs.GetBaseIndex() + assignedIDs.GetEnumValueNames().size())
                {
                    return assignedIDs;
                }
            }
            // Return a default AssignedEnumMsgIDs if not found
            return AssignedEnumMsgIDs(EnumIDs("", {}), -1);
        }
        std::string EnumMsgIDMgr::Serialize() const
        {
            json j;
            j["m_AssignedEnumMsgIDs"] = m_AssignedEnumMsgIDs;
            j["m_NextBaseIndex"] = m_NextBaseIndex;
            return j.dump(4); // 4-space indentation
        }
        void EnumMsgIDMgr::DeSerialize(const std::string& str)
        {
            json j = json::parse(str);
            m_AssignedEnumMsgIDs = j["m_AssignedEnumMsgIDs"].get<std::vector<AssignedEnumMsgIDs>>();
            m_NextBaseIndex = j["m_NextBaseIndex"].get<int>();
        }

        int EnumMsgIDMgr::GetNextID(int numOfIDs)
        {
            int nextID = m_NextBaseIndex;
            m_NextBaseIndex += numOfIDs;
            return nextID;
        }

//////////////////////////////////////////////////////////////

        MsgManager& MsgManager::Get()
        {
            static MsgManager instance;
            return instance;
        }
        bool MsgManager::AddToMsgCreators(std::string enumIDsName, IMsgCreator* creator)
        {
            if(m_MsgCreators.find(enumIDsName) != m_MsgCreators.end())
            {
                return false; // Creator for this enumIDs already exists
            }
            m_MsgCreators[enumIDsName] = creator;
            return true;
        }

        IMsgCreator* MsgManager::GetMsgCreator(CE::tcp::MsgPacket& packet) const
        {
            int absID = packet.GetAbsID();
            AssignedEnumMsgIDs assignedIDs = GetEnumIDsFor(absID);
            if(assignedIDs.GetBaseIndex() == -1)
            {
                return nullptr; // No matching enumIDs found
            }
            std::string enumIDsName = assignedIDs.GetEnumName();
            auto it = m_MsgCreators.find(enumIDsName);
            if(it != m_MsgCreators.end())
            {
                return it->second; // Return the creator for the found enumIDs
            }
            return nullptr; // No creator found for the enumIDs
        }

        IMsgCreator* MsgManager::GetMsgCreator(std::string enumIDsName) const
        {
            auto it = m_MsgCreators.find(enumIDsName);
            if(it != m_MsgCreators.end())            
            {
                return it->second;  
            }
            return nullptr;
        }

        bool MsgManager::AddToMsgProcessors(std::string enumIDsName, IMsgProcessor* processor)
        {
            auto it = m_MsgProcessors.find(enumIDsName);
            if(it != m_MsgProcessors.end())
            {
                it->second.push_back(processor);
                return true;
            }
            m_MsgProcessors[enumIDsName] = {processor};
            return true;
        }
        std::vector<IMsgProcessor*> MsgManager::GetMsgProcessors(std::string enumIDsName) const
        {
            auto it = m_MsgProcessors.find(enumIDsName);
            if(it != m_MsgProcessors.end())
            {
                return it->second;
            }
            return {};
        }
        
    }