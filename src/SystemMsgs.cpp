#include <SystemMsgs.h>
#include <Msg.h>

#include <SystemMsgConstants.cs.h>
#include <MsgManager.h>
using namespace SharedSysMsgConstants;

namespace CE::tcp

{
 
    TCPSystemMsgs::TCPSystemMsgs() : TCPSystemMsgCommandsName("TCPSystemCommands")
    {
        TCPSystemMsgEnumIDs = EnumIDs(TCPSystemMsgCommandsName, SharedSysMsgConstants::SystemCmdNames);
    }

    EnumIDs TCPSystemMsgs::GetTCPSystemMsgEnumIDs()
    {
        return TCPSystemMsgEnumIDs;
    }   

    Msg* TCPSystemMsgs::CreateMsg(MsgPacket& packet)
    {
        switch(packet.GetMsgID())
        {
            case MsgManager::IntHashOfStr("CommunicationFailedCmd"):
                return  new CommunicationFailedMsg(packet);
            case MsgManager::IntHashOfStr("EnableEncryptDecryptCmd"):
                return  new EnableEncryptDecryptMsg(packet);
            case MsgManager::IntHashOfStr("RequestKeyAndIVCmd"):
                return new RequestKeyAndIVMsg(packet);
            case MsgManager::IntHashOfStr("UpdateKeyAndIVCmd"):
                return new UpdateKeyAndIVMsg(packet); 
            case MsgManager::IntHashOfStr("KeepAliveCmd"):
                return new KeepAliveMsg(packet);
            case MsgManager::IntHashOfStr("RemoteConnectionOkCmd"):
                return new RemoteConnectionOkMsg(packet);
            case MsgManager::IntHashOfStr("AutherizationStartRequestCmd"):
                return new AutherizationStartRequestMsg(packet);
            case MsgManager::IntHashOfStr("AutherizationReadyCmd"):
                return new AutherizationReadyMsg(packet);
            case MsgManager::IntHashOfStr("AutherizationValidationCmd"):
                return new AutherizationValidationMsg(packet);
            case MsgManager::IntHashOfStr("ValidateIVCmd"):
                return new ValidateIVMsg(packet);
            case MsgManager::IntHashOfStr("ValidateIVResultCmd"):
                return new ValidateIVResultMsg(packet);
            case MsgManager::IntHashOfStr("AddToCmdInfoCmd"):
                return new AddToCmdInfoMsg(packet);
            case MsgManager::IntHashOfStr("DebugQueryCmd"):
                return new DebugQueryMsg(packet);
            case MsgManager::IntHashOfStr("DebugResponseCmd"):
                return new DebugResponseMsg(packet);
            
            default:
                break;
        }
        return nullptr;
    }
    
    RemoteConnectionTestMsg::RemoteConnectionTestMsg() : Msg("RemoteConnectionTestCmd")
    {
    }

    RemoteConnectionTestMsg::RemoteConnectionTestMsg(MsgPacket& packet) : Msg("RemoteConnectionTestCmd", packet)
    {
    }

    RemoteConnectionTestMsg::~RemoteConnectionTestMsg()
    {
    }

    //////////////////////////////////////

    CommunicationFailedMsg::CommunicationFailedMsg() : Msg("CommunicationFailedCmd")
    {
        // never encrypted
        SetNeverEncrypted();
    }
    CommunicationFailedMsg::CommunicationFailedMsg(MsgPacket& packet) : Msg("CommunicationFailedCmd", packet)
    {
        // never encrypted
        SetNeverEncrypted();
        DeSerializeBody();
    }
    CommunicationFailedMsg::~CommunicationFailedMsg()
    {
    }

    void CommunicationFailedMsg::SerializeBody()
    {
        json j = *this;
        std::string jsonStr = j.dump(1); // 
        m_MsgPacket.SetMsgBodySize( jsonStr.size());
        m_MsgPacket.SetBodyDataFromStr( jsonStr);   
    }

    void CommunicationFailedMsg::DeSerializeBody()
    {
        json jsonBody = json::parse(m_MsgPacket.GetBodyDataAsStr());
        REASON_CODE = jsonBody["REASON_CODE"].get<int>();
    }



////////////////////////////////////////////////////////////////////
    
    AvailableCmdInfoMsg::AvailableCmdInfoMsg() : Msg("AvailableCmdInfoCmd")
    {
    }
    AvailableCmdInfoMsg::AvailableCmdInfoMsg(MsgPacket& packet) : Msg("AvailableCmdInfoCmd", packet)
    {
    }
    AvailableCmdInfoMsg::~AvailableCmdInfoMsg()
    {
    }   

    void AvailableCmdInfoMsg::SerializeBody()
    {
    }

    void AvailableCmdInfoMsg::DeSerializeBody()
    {
    }

//////////////////////////////////////////////////////////////////////

    AddToCmdInfoMsg::AddToCmdInfoMsg() : Msg("AddToCmdInfoCmd")
    {
    }
    AddToCmdInfoMsg::AddToCmdInfoMsg(MsgPacket& packet) : Msg("AddToCmdInfoCmd", packet)
    {
    }
    AddToCmdInfoMsg::~AddToCmdInfoMsg()
    {
    }

    void AddToCmdInfoMsg::AddInfo(const std::string& categoryName, const std::vector<std::string>& cmdNames)
    {
        CMD_INFOS_TO_ADD[categoryName] = cmdNames;
    }

    void AddToCmdInfoMsg::SerializeBody()
    {
        SetAcknowledgeMsgID(UpdateAvailableCmdInfoCmd); // After adding new cmd info, sender expects to receive updated full cmd info list

        json j = *this;
        std::string jsonStr = j.dump(1); // 
        m_MsgPacket.SetMsgBodySize( jsonStr.size());
        m_MsgPacket.SetBodyDataFromStr( jsonStr);   
    }

    void AddToCmdInfoMsg::DeSerializeBody()
    {
        json jsonBody = json::parse(m_MsgPacket.GetBodyDataAsStr());
        CMD_INFOS_TO_ADD = jsonBody["CMD_INFOS_TO_ADD"].get<std::map<std::string, std::vector<std::string>>>();
    }

///////////////////////////////////////////////////////////////////////
    UpdateAvailableCmdInfoMsg::UpdateAvailableCmdInfoMsg() : Msg("UpdateAvailableCmdInfoCmd")
    {
    }
    UpdateAvailableCmdInfoMsg::UpdateAvailableCmdInfoMsg(MsgPacket& packet) : Msg("UpdateAvailableCmdInfoCmd", packet)
    {
    }
    UpdateAvailableCmdInfoMsg::~UpdateAvailableCmdInfoMsg()
    {
    }
    
    void UpdateAvailableCmdInfoMsg::SerializeBody()
    {

    }

    void UpdateAvailableCmdInfoMsg::DeSerializeBody()
    {
    }
///////////////////////////////////////////////////////////////////////


    AutherizationStartRequestMsg::AutherizationStartRequestMsg() : 
        Msg("AutherizationStartRequestCmd")
    {
        m_MsgPacket.SetIsEncrypted(false); // This message should not be encrypted
    }
    AutherizationStartRequestMsg::AutherizationStartRequestMsg(MsgPacket& packet) : 
        Msg("AutherizationStartRequest", packet)
    {
        DeSerializeBody();
    }
    AutherizationStartRequestMsg::~AutherizationStartRequestMsg()
    {
    }

    void AutherizationStartRequestMsg::DeSerializeBody()
    {
        json bodyJson = json::parse(m_MsgPacket.GetBodyDataAsStr());
        CLIENT_AUTH_ID = bodyJson["CLIENT_AUTH_ID"].get<std::string>();        
    }

    void AutherizationStartRequestMsg::SerializeBody()
    {

        json j = *this;
        std::string jsonStr = j.dump(1); // 
        m_MsgPacket.SetMsgBodySize( jsonStr.size());
        m_MsgPacket.SetBodyDataFromStr( jsonStr);   

    }
    //////////////////////////////////////////////////////////////////

    AutherizationReadyMsg::AutherizationReadyMsg() : Msg("AutherizationReadyCmd")
    {
    }

    AutherizationReadyMsg::AutherizationReadyMsg(MsgPacket& packet) : Msg("AutherizationReadyCmd", packet)
    {
    }

    AutherizationReadyMsg::~AutherizationReadyMsg()
    {

    }

    ////////////////////////////////////////////////////////////////

    AutherizationValidationMsg::AutherizationValidationMsg() : Msg("AutherizationValidationCmd")
    {
    }
    AutherizationValidationMsg::AutherizationValidationMsg(MsgPacket& packet) : Msg("AutherizationValidationCmd", packet)
    {
        DeSerializeBody();
    }
    AutherizationValidationMsg::~AutherizationValidationMsg()
    {
    }
    void AutherizationValidationMsg::SerializeBody()
    {
        json j = *this;
        std::string jsonStr = j.dump(1); // 
        m_MsgPacket.SetMsgBodySize( jsonStr.size());
        
        m_MsgPacket.SetBodyDataFromStr( jsonStr);   
    }
    void AutherizationValidationMsg::DeSerializeBody()
    {
        json jsonBody = json::parse(m_MsgPacket.GetBodyDataAsStr());
        IV_TO_VALIDATE = jsonBody["IV_TO_VALIDATE"].get<CryptoBlockVector>();
    }

    ////////////////////////////////////////////////////////////////
    AutherizationValidationResponseMsg::AutherizationValidationResponseMsg() : Msg("AutherizationValidationResponseCmd")
    {
    }
    AutherizationValidationResponseMsg::AutherizationValidationResponseMsg(MsgPacket& packet) : Msg("AutherizationValidationResponseCmd", packet)
    {
        DeSerializeBody();
    }
    AutherizationValidationResponseMsg::~AutherizationValidationResponseMsg()
    {
    }
    void AutherizationValidationResponseMsg::SerializeBody()
    {
        json j = *this;
        std::string jsonStr = j.dump(1); // 
        m_MsgPacket.SetMsgBodySize( jsonStr.size());
        m_MsgPacket.SetBodyDataFromStr( jsonStr);   
    }
    void AutherizationValidationResponseMsg::DeSerializeBody()
    {
        json jsonBody = json::parse(m_MsgPacket.GetBodyDataAsStr());
        IV_IS_VALID = jsonBody["IV_IS_VALID"].get<bool>();
    }

    ////////////////////////////////////////////////////////////////
    AutherizationEndMsg::AutherizationEndMsg() : Msg("AutherizationEndCmd")
    {
    }   
    AutherizationEndMsg::AutherizationEndMsg(MsgPacket& packet) : Msg("AutherizationEndCmd", packet)
    {
    }
    AutherizationEndMsg::~AutherizationEndMsg()
    {
    }
    ////////////////////////////////////////////////////////////////

    EnableEncryptDecryptMsg::EnableEncryptDecryptMsg() : Msg("EnableEncryptDecryptCmd")
    {
    }

    EnableEncryptDecryptMsg::EnableEncryptDecryptMsg(MsgPacket& packet) : 
        Msg("EnableEncryptDecryptCmd", packet)
    {
    }

    EnableEncryptDecryptMsg::~EnableEncryptDecryptMsg()
    {
    }



    //////////////////////////////////////////////////////////////////////ValidateIVMsg/

    ValidateIVMsg::ValidateIVMsg() : Msg("ValidateIVCmd")
    {
    }
    ValidateIVMsg::ValidateIVMsg(MsgPacket& packet) : 
        Msg("ValidateIVCmd", packet)
    {
        DeSerializeBody();
    }

    ValidateIVMsg::~ValidateIVMsg()
    {
    }

    void ValidateIVMsg::SerializeBody()
    {
        json j = *this;
        std::string jsonStr = j.dump(1); // 
        m_MsgPacket.SetMsgBodySize( jsonStr.size());
        m_MsgPacket.SetBodyDataFromStr( jsonStr);
    }

    void ValidateIVMsg::DeSerializeBody()
    {
        json jsonBody = json::parse(m_MsgPacket.GetBodyDataAsStr());
        IV_TO_VALIDATE = jsonBody["IV_TO_VALIDATE"].get<CryptoBlockVector>();
    }

    void ValidateIVMsg::SetIVToValidate(CryptoBlockVector iv)
    {
        IV_TO_VALIDATE = iv;
    }
    CryptoBlockVector ValidateIVMsg::GetIVToValidate()
    {
        return IV_TO_VALIDATE;
    }

    //////////////////////////////////////////////////////////////////////ValidateIVMsg/
    ValidateIVResultMsg::ValidateIVResultMsg() : Msg( "ValidateIVResultCmd")
    {
    }

    ValidateIVResultMsg::ValidateIVResultMsg(MsgPacket& packet) : 
        Msg("ValidateIVResultCmd", packet)
    {
        DeSerializeBody();
    }

    ValidateIVResultMsg::~ValidateIVResultMsg()
    {
    }

    void ValidateIVResultMsg::DeSerializeBody()
    {

        json jsonBody = json::parse(m_MsgPacket.GetBodyDataAsStr());
        IV_IS_VALID = jsonBody["IV_IS_VALID"].get<bool>();
        
    }
    void ValidateIVResultMsg::SerializeBody()
    {
        json j = *this;
        std::string JSONStr = j.dump(1); // 
        m_MsgPacket.SetMsgBodySize( JSONStr.size());
        m_MsgPacket.SetBodyDataFromStr( JSONStr);

    }

    void ValidateIVResultMsg::SetIsValid(bool isValid)
    {
        IV_IS_VALID = isValid;
    }
    bool ValidateIVResultMsg::GetIsValid()
    {
        return IV_IS_VALID;
    }   

    ///////////////////////////////////////////////////////////////////////////////////

    UpdateKeyAndIVMsg::UpdateKeyAndIVMsg() : Msg("UpdateKeyAndIVCmd")
    {
        CreateNewKeyAndIV();
    }

    UpdateKeyAndIVMsg::UpdateKeyAndIVMsg(MsgPacket& packet) : 
        Msg("UpdateKeyAndIVCmd", packet)
    {
        DeSerializeBody();
    }

    UpdateKeyAndIVMsg::~UpdateKeyAndIVMsg()
    {
    }

    void UpdateKeyAndIVMsg::SerializeBody()
    {
        json j = *this;
        std::string JSONStr = j.dump(1);
        m_MsgPacket.SetMsgBodySize( JSONStr.size());
        m_MsgPacket.SetBodyDataFromStr( JSONStr);

    }

    void UpdateKeyAndIVMsg::DeSerializeBody()
    {
        json jsonBody = json::parse(m_MsgPacket.GetBodyDataAsStr());
        IV_AND_KEY = jsonBody["IV_AND_KEY"].get<IVAndKeyValues>();


//        JSONDocumentWrapper docWrapper(*(new Document()));
//        docWrapper.Parse( m_MsgPacket.GetBodyDataAsStr());

//        std::string keyStr = docWrapper.GetString("Key");
//        std::string ivStr = docWrapper.GetString("IV");

//        IV_AND_KEY.key.Base64Decode(keyStr);
//        IV_AND_KEY.iv.Base64Decode(ivStr);
    }

    void UpdateKeyAndIVMsg::CreateNewKeyAndIV()
    {
        IV_AND_KEY = AESAccessManagement::Get()->CreateIVAndKey();
    }

    CryptoBlockVector UpdateKeyAndIVMsg::GetIV()
    {
        return IV_AND_KEY.IV;
    }   

    CryptoBlockVector UpdateKeyAndIVMsg::GetKey()
    {
        return IV_AND_KEY.KEY;
    }

    IVAndKeyValues& UpdateKeyAndIVMsg::GetIVAndKeyValues()
    {
        return IV_AND_KEY;
    }
    //////////////////////////////////////////////////////////////////////ValidateIVMsg/
    DebugQueryMsg::DebugQueryMsg() : Msg("DebugQueryCmd") 
    {
    }

    DebugQueryMsg::DebugQueryMsg(MsgPacket& packet) : 
        Msg("DebugQueryCmd", packet)
    {
        DeSerializeBody();
    }

    DebugQueryMsg::~DebugQueryMsg()
    {
    }
    void DebugQueryMsg::SerializeBody()
    {
        json j = *this;
        std::string JSONStr = j.dump(1);
        m_MsgPacket.SetMsgBodySize( JSONStr.size());
        m_MsgPacket.SetBodyDataFromStr( JSONStr);
    }
    void DebugQueryMsg::DeSerializeBody()
    {
        json jsonBody = json::parse(m_MsgPacket.GetBodyDataAsStr());
        QUERY_STR = jsonBody["QUERY_STR"].get<std::string>();

    }

    void DebugQueryMsg::SetQueryStr(std::string queryStr)
    {
        QUERY_STR = queryStr;
    }
    std::string DebugQueryMsg::GetQueryStr()
    {
        return QUERY_STR;
    }


    ////////////////////////////////////////////////////////////////////
    DebugResponseMsg::DebugResponseMsg() : Msg("DebugResponseCmd")
    {
    }   
    DebugResponseMsg::DebugResponseMsg(MsgPacket& packet) : 
        Msg("DebugResponseCmd", packet)
    {
        DeSerializeBody();
    }
    DebugResponseMsg::~DebugResponseMsg()
    {
    }
    void DebugResponseMsg::SerializeBody()
    {
        json j = *this;
        std::string JSONStr = j.dump(1);
        m_MsgPacket.SetMsgBodySize( JSONStr.size());
        
        m_MsgPacket.SetBodyDataFromStr( JSONStr);
    }
    void DebugResponseMsg::DeSerializeBody()
    {

        json jsonBody = json::parse(m_MsgPacket.GetBodyDataAsStr());
        RESPONSE_STR = jsonBody["RESPONSE_STR"].get<std::string>();

    }

    void DebugResponseMsg::SetResponseStr(std::string responseStr)
    {
        RESPONSE_STR = responseStr;
    }
    std::string DebugResponseMsg::GetResponseStr()
    {
        return RESPONSE_STR;
    }


    ////////////////////////////////////////////////////////////
    KeepAliveMsg::KeepAliveMsg() : Msg("KeepAlive")
    {
    }
    KeepAliveMsg::KeepAliveMsg(MsgPacket& packet) : 
        Msg("KeepAlive", packet)
    {
    }
    KeepAliveMsg::~KeepAliveMsg()
    {
    }


    ///////////////////////////////////////////////////
    RemoteConnectionOkMsg::RemoteConnectionOkMsg() : Msg("RemoteConnectionOk")
    {
    }   
    RemoteConnectionOkMsg::RemoteConnectionOkMsg(MsgPacket& packet) : 
        Msg("RemoteConnectionOk", packet)
    {
    }
    RemoteConnectionOkMsg::~RemoteConnectionOkMsg()
    {
    }


    ////////////////////////////////////////////////////////////////////
    RequestKeyAndIVMsg::RequestKeyAndIVMsg() : Msg("RequestKeyAndIV")
    {
    }
    RequestKeyAndIVMsg::RequestKeyAndIVMsg(MsgPacket& packet) : 
        Msg("RequestKeyAndIV", packet)
    {
    }
    RequestKeyAndIVMsg::~RequestKeyAndIVMsg()
    {
    }
}

