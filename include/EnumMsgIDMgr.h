#ifndef ENUM_MSG_ID_MGR_H
#define ENUM_MSG_ID_MGR_H

#include <string>
#include <vector>
#include <map>
#include <EnumIDs.h>
#include <Msg.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json_abi_v3_12_0::json;

namespace Test
{


class EnumMsgIDMgr : public IEnumIDMgr
{
    public:
        EnumMsgIDMgr() = default;
        ~EnumMsgIDMgr() = default;

        int AddEnumIDs(const EnumIDs& enumIDs) override;
        AssignedEnumMsgIDs GetEnumIDsFor(int absID) const override;
        std::string Serialize() const;
        void DeSerialize(const std::string& str);

    protected:
        int GetNextID(int numOfIDs) override;
    private:
        std::vector<AssignedEnumMsgIDs> m_AssignedEnumMsgIDs;
        int m_NextBaseIndex = 0;
};

class IMsgCreator
{
    public:
        virtual CE::tcp::Msg* CreateMsg(CE::tcp::MsgPacket& packet) = 0;
};

class IMsgProcessor
{
    public:
        virtual bool ProcessMsg(CE::tcp::Msg& msg) = 0;
};  

class MsgManager : public EnumMsgIDMgr
{
    public:
        static MsgManager& Get();
        ~MsgManager() = default;

        bool AddToMsgCreators(std::string enumIDsName, IMsgCreator* creator);
        IMsgCreator* GetMsgCreator(CE::tcp::MsgPacket& packet) const;
        IMsgCreator* GetMsgCreator(std::string enumIDsName) const;

        bool AddToMsgProcessors(std::string enumIDsName, IMsgProcessor* processor);
        std::vector<IMsgProcessor*> GetMsgProcessors(std::string enumIDsName) const;

    private:
        MsgManager() = default;
        std::map<std::string, IMsgCreator*> m_MsgCreators; // enumIDsName -> creator
        std::map<std::string, std::vector<IMsgProcessor*>> m_MsgProcessors; // enumIDsName -> list of processors
};

} // namespace Test

#endif // ENUM_MSG_ID_MGR_H