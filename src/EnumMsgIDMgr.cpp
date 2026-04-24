#include <EnumMsgIDMgr.h>
#include <SystemMsgs.h>

namespace CE::tcp
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
                if(absID >= assignedIDs.GetBaseIndex() && absID < assignedIDs.GetBaseIndex() + (int)assignedIDs.GetEnumValueNames().size())
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
            return j.dump(1); // 
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

        int MsgManager::AddEnumIDs(const EnumIDs& enumIDs)
        {
            if( m_AssignedEnumMsgIDs.size() == 0 )
            {
                // make sure SystemMsgs is initialized and registered as a MsgCreator
                TCPSystemMsgs& systemMsgs =  TCPSystemMsgs::GetInstance(); 
                MsgProcessor::AddMsgCreator((ICreateMsgFromPacket*) &TCPSystemMsgs::GetInstance());
                EnumMsgIDMgr::AddEnumIDs(systemMsgs.GetTCPSystemMsgEnumIDs());
            }
            return EnumMsgIDMgr::AddEnumIDs(enumIDs);
        }

        bool MsgManager::AddToMsgCreators(std::string enumIDsName, ICreateMsgFromPacket* creator)
        {
            if(m_MsgCreators.find(enumIDsName) != m_MsgCreators.end())
            {
                return false; // Creator for this enumIDs already exists
            }
            m_MsgCreators[enumIDsName] = creator;
            return true;
        }

        ICreateMsgFromPacket* MsgManager::GetMsgCreator(CE::tcp::MsgPacket& packet) const
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

        ICreateMsgFromPacket* MsgManager::GetMsgCreator(std::string enumIDsName) const
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

        std::string MsgManager::GetMsgNameFromID(int absID) const
        {
            AssignedEnumMsgIDs assignedIDs = GetEnumIDsFor(absID);
            if(assignedIDs.GetBaseIndex() == -1)
            {
                return ""; // No matching enumIDs found
            }
            return assignedIDs.GetEnumValueName(absID - assignedIDs.GetBaseIndex());
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