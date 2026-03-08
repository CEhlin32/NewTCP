#include <TCPMsgs.h>
#include <AESSupport.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json_abi_v3_12_0::json;

namespace Test
{

    TCPMsgs& TCPMsgs::GetInstance()
        {
            static TCPMsgs instance;
            return instance;
        }

    TCPMsgs::TCPMsgs() : TCPMsgCommandsName("TCPMsgCommands"), TCPCmdNames({
            "AvailableCmdInfoCmd",
            "EnableEncryptDecryptCmd",
            "RequestKeyAndIVCmd",
            "UpdateKeyAndIVCmd",
            "AutherizationStartRequestCmd",
            "AutherizationReadyCmd",
            "AutherizationValidationCmd",
            "AutherizationValidationResponseCmd",
            "ValidateIVCmd",
            "ValidateIVResultCmd",
            "KeepAliveCmd",
            "RemoteConnectionOkCmd",
            "DebugQueryCmd",
            "DebugResponseCmd"
        }), TCPMsgEnumIDs(TCPMsgCommandsName, TCPCmdNames)
    {
        // Register the TCP message command enums with the MsgManager
        baseID = MsgManager::Get().AddEnumIDs(GetTCPMsgEnumIDs());
    }   
    EnumIDs TCPMsgs::GetTCPMsgEnumIDs()
    {
        return TCPMsgEnumIDs;  
    }

    EnumIDs TCPMsgEnumIDs;
    
        CE::tcp::Msg* TCPMsgs::CreateMsg(CE::tcp::MsgPacket& packet)
        {
            int relID = packet.GetMsgID() - baseID;
            switch(relID)
            {
                case AvailableCmdInfoCmd:
                    return new AvailableCmdInfoMsg(packet);
                case EnableEncryptDecryptCmd:
                    return new EnableEncryptDecryptMsg(packet);
                case RequestKeyAndIVCmd:
                    return new RequestKeyAndIVMsg(packet);
                case UpdateKeyAndIVCmd:
                    return new UpdateKeyAndIVMsg(packet);
                case AutherizationStartRequestCmd:
                    return new AutherizationStartRequestMsg(packet);
                case AutherizationReadyCmd:
                    return new AutherizationReadyMsg(packet);
                case KeepAliveCmd:
                    return new KeepAliveMsg(packet);
                case RemoteConnectionOkCmd:
                    return new RemoteConnectionOkMsg(packet);
                case ValidateIVCmd:
                    return new ValidateIVMsg(packet);
                case ValidateIVResultCmd:
                    return new ValidateIVResultMsg(packet);
                case DebugQueryCmd:
                    return new DebugQueryMsg(packet);
                case DebugResponseCmd:
                    return new DebugResponseMsg(packet);    
                default:
                    return nullptr;
            }
        }

    
    AvailableCmdInfoMsg::AvailableCmdInfoMsg(CE::tcp::MsgPacket& packet) : CE::tcp::Msg("AvailableCmdInfoCmd", TCPMsgs::AvailableCmdInfoCmd)
    {   
        m_MsgPacket = packet;
        DeSerializeBody();
    }

    AvailableCmdInfoMsg::AvailableCmdInfoMsg() : CE::tcp::Msg("AvailableCmdInfoCmd", TCPMsgs::AvailableCmdInfoCmd)
    {
    }

    AvailableCmdInfoMsg::~AvailableCmdInfoMsg()
    {
    }
    
    void AvailableCmdInfoMsg::SerializeBody()
    {
        std::string jsonStr = MsgManager::Get().Serialize();
        m_MsgPacket.SetMsgBodySize( jsonStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( jsonStr);
    }

    void AvailableCmdInfoMsg::DeSerializeBody()
    {
        std::string bodyStr = m_MsgPacket.GetBodyDataAsStr();
        MsgManager::Get().DeSerialize(bodyStr);
    }

/////////////////////////////////////////////////////////////////////////////

        AutherizationStartRequestMsg::AutherizationStartRequestMsg() : 
            CE::tcp::Msg("AutherizationStartRequestMsg", TCPMsgs::AutherizationStartRequestCmd)
        {
        }
        AutherizationStartRequestMsg::AutherizationStartRequestMsg(CE::tcp::MsgPacket& packet) : CE::tcp::Msg("AutherizationStartRequestMsg", TCPMsgs::AutherizationStartRequestCmd)
        {
            m_MsgPacket = packet;
            DeSerializeBody();
        }

        AutherizationStartRequestMsg::~AutherizationStartRequestMsg()
        {

        }

        std::string AutherizationStartRequestMsg::GetInitValue()
        {
            return m_InitAuth;
        }

        void AutherizationStartRequestMsg::SerializeBody() 
        {
            json j = m_InitAuth;
            std::string s = j.dump(4); // 4-space indentation
            m_MsgPacket.SetBodyDataFromStr(s);
        }
        void AutherizationStartRequestMsg::DeSerializeBody()
        {
            std::string bodyStr = m_MsgPacket.GetBodyDataAsStr();
            json j = json::parse(bodyStr);
            m_InitAuth = j.get<std::string>();
        }


/////////////////////////////////////////////////////////////////////////////

    AutherizationReadyMsg::AutherizationReadyMsg() : CE::tcp::Msg("AutherizationReadyCmd", TCPMsgs::AutherizationReadyCmd)
    {

    }

    AutherizationReadyMsg::AutherizationReadyMsg(CE::tcp::MsgPacket& packet) : CE::tcp::Msg("AutherizationReadyCmd", TCPMsgs::AutherizationReadyCmd)
    {
        m_MsgPacket = packet;
        DeSerializeBody();
    }

    AutherizationReadyMsg::~AutherizationReadyMsg()
    {
    }


/////////////////////////////////////////////////////////////////////////////

    KeepAliveMsg::KeepAliveMsg() : CE::tcp::Msg("KeepAliveCmd", TCPMsgs::KeepAliveCmd)
    {
    }
    KeepAliveMsg::KeepAliveMsg(CE::tcp::MsgPacket& packet) : CE::tcp::Msg("KeepAliveCmd", TCPMsgs::KeepAliveCmd)
    {
        m_MsgPacket = packet;
        DeSerializeBody();
    }
    KeepAliveMsg::~KeepAliveMsg()
    {

    }

/////////////////////////////////////////////////////////////////////////////
    RemoteConnectionOkMsg::RemoteConnectionOkMsg() : CE::tcp::Msg("RemoteConnectionOkCmd", TCPMsgs::RemoteConnectionOkCmd)
    {
    }
    RemoteConnectionOkMsg::RemoteConnectionOkMsg(CE::tcp::MsgPacket& packet) : CE::tcp::Msg("RemoteConnectionOkCmd", TCPMsgs::RemoteConnectionOkCmd)
    {
        m_MsgPacket = packet;
        DeSerializeBody();
    }
    RemoteConnectionOkMsg::~RemoteConnectionOkMsg()
    {
    }

/////////////////////////////////////////////////////////////////////////////
    UpdateKeyAndIVMsg::UpdateKeyAndIVMsg() : CE::tcp::Msg("UpdateKeyAndIVCmd", TCPMsgs::UpdateKeyAndIVCmd)
    {
        CreateNewKeyAndIV();
    }
    UpdateKeyAndIVMsg::UpdateKeyAndIVMsg(CE::tcp::MsgPacket& packet) : CE::tcp::Msg("UpdateKeyAndIVCmd", TCPMsgs::UpdateKeyAndIVCmd)
    {
        m_MsgPacket = packet;
        DeSerializeBody();
    }
    UpdateKeyAndIVMsg::~UpdateKeyAndIVMsg()
    {
    }
    void UpdateKeyAndIVMsg::CreateNewKeyAndIV()
    {
        m_IVAndKeyValues = AESAccessManagement::Get()->CreateIVAndKey();
    }

    void UpdateKeyAndIVMsg::SerializeBody()
    {

            json j;
            j["IV"] = m_IVAndKeyValues.IV;
            j["Key"] = m_IVAndKeyValues.KEY;
            std::string s = j.dump(4); // 4-space indentation
            m_MsgPacket.SetBodyDataFromStr(s);
        }

    void UpdateKeyAndIVMsg::DeSerializeBody()
    {
        std::string bodyStr = m_MsgPacket.GetBodyDataAsStr();
        json j = json::parse(bodyStr);
        m_IVAndKeyValues.IV = j["IV"].get<CryptoBlockVector>();
        m_IVAndKeyValues.KEY = j["Key"].get<CryptoBlockVector>();
    }
/////////////////////////////////////////////////////////////////////////////
    ValidateIVMsg::ValidateIVMsg() : CE::tcp::Msg("ValidateIVCmd", TCPMsgs::ValidateIVCmd)
    {
    }
    ValidateIVMsg::ValidateIVMsg(CE::tcp::MsgPacket& packet) : CE::tcp::Msg("ValidateIVCmd", TCPMsgs::ValidateIVCmd)
    {
        m_MsgPacket = packet;
        DeSerializeBody();
    }
    ValidateIVMsg::~ValidateIVMsg()
    {
    }
    void ValidateIVMsg::SerializeBody()
    {
        json j;
        j["IVToValidate"] = IVToValidate;
        std::string s = j.dump(4); // 4-space indentation
        m_MsgPacket.SetBodyDataFromStr(s);
    }
    void ValidateIVMsg::DeSerializeBody()
    {
        std::string bodyStr = m_MsgPacket.GetBodyDataAsStr();
        json j = json::parse(bodyStr);
        IVToValidate = j["IVToValidate"].get<CryptoBlockVector>();
    }
    void ValidateIVMsg::SetIVToValidate(CryptoBlockVector iv)
    {
        IVToValidate = iv;
    }
    CryptoBlockVector ValidateIVMsg::GetIVToValidate()
    {
        return IVToValidate;
    }

/////////////////////////////////////////////////////////////////////////////
    ValidateIVResultMsg::ValidateIVResultMsg() : CE::tcp::Msg("ValidateIVResultCmd", TCPMsgs::ValidateIVResultCmd)
    {
    }
    ValidateIVResultMsg::ValidateIVResultMsg(CE::tcp::MsgPacket& packet) : CE::tcp::Msg("ValidateIVResultCmd", TCPMsgs::ValidateIVResultCmd)
    {
        m_MsgPacket = packet;
        DeSerializeBody();
    }
    ValidateIVResultMsg::~ValidateIVResultMsg()
    {
    }
    void ValidateIVResultMsg::SerializeBody()
    {
        json j;
        j["IsValid"] = IsValid;
        std::string s = j.dump(4); // 4-space indentation
        m_MsgPacket.SetBodyDataFromStr(s);
    }
    void ValidateIVResultMsg::DeSerializeBody()
    {
        std::string bodyStr = m_MsgPacket.GetBodyDataAsStr();
        json j = json::parse(bodyStr);
        IsValid = j["IsValid"].get<bool>();
    }
    void ValidateIVResultMsg::SetIsValid(bool isValid)
    {
        IsValid = isValid;
    }
    bool ValidateIVResultMsg::GetIsValid()
    {

        return IsValid;
    }

/////////////////////////////////////////////////////////////////////////////
    DebugQueryMsg::DebugQueryMsg() : CE::tcp::Msg("DebugQueryCmd", TCPMsgs::DebugQueryCmd)
    {
    }
    DebugQueryMsg::DebugQueryMsg(CE::tcp::MsgPacket& packet) : CE::tcp::Msg("DebugQueryCmd", TCPMsgs::DebugQueryCmd)
    {
        m_MsgPacket = packet;
        DeSerializeBody();
    }
    DebugQueryMsg::~DebugQueryMsg()
    {
    }
    void DebugQueryMsg::SerializeBody()
    {
        json j;
        j["QueryStr"] = QueryStr;
        std::string s = j.dump(4); // 4-space indentation
        m_MsgPacket.SetBodyDataFromStr(s);
    }
    void DebugQueryMsg::DeSerializeBody()
    {
        std::string bodyStr = m_MsgPacket.GetBodyDataAsStr();
        json j = json::parse(bodyStr);
        QueryStr = j["QueryStr"].get<std::string>();
    }
    void DebugQueryMsg::SetQueryStr(std::string queryStr)
    {
        QueryStr = queryStr;
    }
    std::string DebugQueryMsg::GetQueryStr()
    {        return QueryStr;
    }
/////////////////////////////////////////////////////////////////////////////
    DebugResponseMsg::DebugResponseMsg() : CE::tcp::Msg("DebugResponseCmd", TCPMsgs::DebugResponseCmd)
    {
    }
    DebugResponseMsg::DebugResponseMsg(CE::tcp::MsgPacket& packet) : CE::tcp::Msg("DebugResponseCmd", TCPMsgs::DebugResponseCmd)
    {
        m_MsgPacket = packet;
        DeSerializeBody();
    }
    DebugResponseMsg::~DebugResponseMsg()
    {
    }
    void DebugResponseMsg::SerializeBody()
    {
        json j;
        j["ResponseStr"] = ResponseStr;
        std::string s = j.dump(4); // 4-space indentation
        m_MsgPacket.SetBodyDataFromStr(s);
    }
    void DebugResponseMsg::DeSerializeBody()
    {
        std::string bodyStr = m_MsgPacket.GetBodyDataAsStr();
        json j = json::parse(bodyStr);
        ResponseStr = j["ResponseStr"].get<std::string>();
    }
    void DebugResponseMsg::SetResponseStr(std::string responseStr)
    {
        ResponseStr = responseStr;
    }
    std::string DebugResponseMsg::GetResponseStr()
    {        
        return ResponseStr;
    }
/////////////////////////////////////////////////////////////////////////////
    EnableEncryptDecryptMsg::EnableEncryptDecryptMsg() : CE::tcp::Msg("EnableEncryptDecryptCmd", TCPMsgs::EnableEncryptDecryptCmd)
    {
    }
    EnableEncryptDecryptMsg::EnableEncryptDecryptMsg(CE::tcp::MsgPacket& packet) : CE::tcp::Msg("EnableEncryptDecryptCmd", TCPMsgs::EnableEncryptDecryptCmd)
    {
        m_MsgPacket = packet;
        DeSerializeBody();
    }
    EnableEncryptDecryptMsg::~EnableEncryptDecryptMsg()
    {
    }
    void EnableEncryptDecryptMsg::SerializeBody()
    {
        json j;
        j["NextIV"] = m_nextIV;
        std::string s = j.dump(4); // 4-space indentation
        m_MsgPacket.SetBodyDataFromStr(s);
    }
    void EnableEncryptDecryptMsg::DeSerializeBody()
    {
        std::string bodyStr = m_MsgPacket.GetBodyDataAsStr();
        json j = json::parse(bodyStr);
        m_nextIV = j["NextIV"].get<CryptoBlockVector>();  
    }
    CryptoBlockVector EnableEncryptDecryptMsg::GetEncryptionIV()
    {
        if( true == m_MsgPacket.GetIsEncrypted())
            return m_nextIV;

        return defaultIV;
    }

    /////////////////////////////////////////////////////////////////
    RequestKeyAndIVMsg::RequestKeyAndIVMsg() : CE::tcp::Msg("RequestKeyAndIVCmd", TCPMsgs::RequestKeyAndIVCmd)
    {
    }
    RequestKeyAndIVMsg::RequestKeyAndIVMsg(CE::tcp::MsgPacket& packet) : CE::tcp::Msg("RequestKeyAndIVCmd", TCPMsgs::RequestKeyAndIVCmd)
    {
        m_MsgPacket = packet;
        DeSerializeBody();
    }
    RequestKeyAndIVMsg::~RequestKeyAndIVMsg()
    {
    }
////////////////////////////////////////////////////////////////////////

}
