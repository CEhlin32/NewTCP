#ifndef ENUM_MSG_ID_MGR_H
#define ENUM_MSG_ID_MGR_H

#include <string>
#include <vector>
#include <map>
#include <EnumIDs.h>
#include <Msg.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json_abi_v3_12_0::json;
#ifdef OLD
namespace CE::tcp
{


class EnumMsgIDMgr : public IEnumIDMgr
{
    public:
        EnumMsgIDMgr() = default;
        ~EnumMsgIDMgr() = default;

        int AddEnumIDs(const EnumIDs& enumIDs) override;
        AssignedEnumMsgIDs GetEnumIDsFor(int absID) const override;

        bool IsValidID(int absID) const override;
        std::string Serialize() const;
        void DeSerialize(const std::string& str);

    protected:
        int GetNextID(int numOfIDs) override;
        std::vector<AssignedEnumMsgIDs> m_AssignedEnumMsgIDs;
    private:
        int m_NextBaseIndex = 0;
};


class MsgManager : public EnumMsgIDMgr
{
    public:



        static MsgManager& Get();
        ~MsgManager() = default;

        int AddEnumIDs(const EnumIDs& enumIDs) override;

        bool AddToMsgCreators(std::string enumIDsName, ICreateMsgFromPacket* creator);
        ICreateMsgFromPacket* GetMsgCreator(CE::tcp::MsgPacket& packet) const;
        ICreateMsgFromPacket* GetMsgCreator(std::string enumIDsName) const;

        bool AddToMsgProcessors(std::string enumIDsName, IMsgProcessor* processor);
        std::vector<IMsgProcessor*> GetMsgProcessors(std::string enumIDsName) const;
        std::string GetMsgNameFromID(int absID) const;
        
    private:


        MsgManager() = default;
        std::map<std::string, ICreateMsgFromPacket*> m_MsgCreators; // enumIDsName -> creator
        std::map<std::string, std::vector<IMsgProcessor*>> m_MsgProcessors; // enumIDsName -> list of processors
};

} // namespace CE::tcp

#endif // OLD
#endif // ENUM_MSG_ID_MGR_H