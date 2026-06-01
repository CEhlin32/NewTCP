#ifdef OLD_CODE
#include "Msg.h"
#include "MsgPacket.h"
#include "SystemMsgConstants.cs.h"

namespace CE::tcp
{

    class SystemMsgManager : public EnumExtenderManager
    {


    public:
        using ProcessMsg = std::function<bool(Msg&)>;
        using CreateMsg = std::function<bool(MsgPacket&)>;

    private:
        std::map<int, std::vector<CreateMsg*>> MsgCreators;
        std::map<int, std::vector<ProcessMsg*>> MsgProcessors;

    public:        
        static SystemMsgManager& Get()
        {
            static SystemMsgManager instance;
            return instance;
        }
        SystemMsgManager() : EnumExtenderManager("SystemMsgManager")
        {
            AddEnumExtender("TCPSystemCommands", SharedSysMsgConstants::SystemCmdNames);
        }

        
        void AddMsgProcessor(int baseID, ProcessMsg& msgProcessor)
        {
            MsgProcessors[baseID].push_back(&msgProcessor);
        }
        void AddMsgCreator(int baseID, CreateMsg& msgCreator)
        {
            MsgCreators[baseID].push_back(&msgCreator);
        }

        int GetBaseIDFromEnumName(std::string enumIDsName) const
        {
            auto it = EnumNameToBaseID.find(enumIDsName);
            if (it != EnumNameToBaseID.end())
            {
                return it->second;
            }
            return -1; // Not found
        }   
        int GetBaseIDForProcessor(ProcessMsg& msgProcessor) const
        {
            for(const auto& pair : MsgProcessors)
            {
                const auto& processors = pair.second;
                if(std::find(processors.begin(), processors.end(), &msgProcessor) != processors.end())
                {
                    return pair.first;
                }
            }
            return -1; // Not found
        }

        int GetBaseIDForCreator(CreateMsg& msgCreator) const
        {
            for(const auto& pair : MsgCreators)
            {
                const auto& creators = pair.second;
                if(std::find(creators.begin(), creators.end(), &msgCreator) != creators.end())
                {
                    return pair.first;
                }
            }
            return -1; // Not found
        }
    };
}
#endif