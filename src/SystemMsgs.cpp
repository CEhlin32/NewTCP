#include <SystemMsgs.h>
#include <Msg.h>
#include <MsgCmds.h>
#include <EnumExtender.h>   
namespace CE::tcp

{
    AvailableCmdInfoMsg::AvailableCmdInfoMsg() : Msg("AvailableCmdInfoCmd", AvailableCmdInfoCmd)
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
        json j = TCPMsgEnumManager::Get();
        std::string jsonStr = j.dump(4); // 4-space indentation
        m_MsgPacket.SetMsgBodySize( jsonStr.size());

#if OLD_SERAILIZE        
        std::string jsonStr = TCPMsgEnumManager::Get().Serialize();
        m_MsgPacket.SetMsgBodySize( jsonStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( jsonStr);
#endif        
    }

    void AvailableCmdInfoMsg::DeSerializeBody()
    {
        std::string bodyStr = m_MsgPacket.GetBodyDataAsStr();
        TCPMsgEnumManager::Get().DeSerialize(bodyStr);
    }
    
///////////////////////////////////////////////////////////////////////


    AutherizationStartRequestMsg::AutherizationStartRequestMsg() : 
        Msg("AutherizationStartRequestCmd", AutherizationStartRequestCmd)
    {
    }
    AutherizationStartRequestMsg::AutherizationStartRequestMsg(MsgPacket& packet) : 
        Msg("AutherizationStartRequest", packet)
    {
    }
    AutherizationStartRequestMsg::~AutherizationStartRequestMsg()
    {
    }

    void AutherizationStartRequestMsg::DeSerializeBody()
    {
        json bodyJson = json::parse(m_MsgPacket.GetBodyDataAsStr());
        m_InitAuth = bodyJson["AuthInitCode"].get<std::string>();        
    }

    void AutherizationStartRequestMsg::SerializeBody()
    {

        json j = *this;
        std::string jsonStr = j.dump(4); // 4-space indentation
        m_MsgPacket.SetMsgBodySize( jsonStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( jsonStr);   

    }
    //////////////////////////////////////////////////////////////////

    AutherizationReadyMsg::AutherizationReadyMsg() : Msg("AutherizationReadyCmd", AutherizationReadyCmd)
    {

    }

    AutherizationReadyMsg::AutherizationReadyMsg(MsgPacket& packet) : Msg("AutherizationReadyCmd", packet)
    {
    }

    AutherizationReadyMsg::~AutherizationReadyMsg()
    {

    }
    ////////////////////////////////////////////////////////////////

    EnableEncryptDecryptMsg::EnableEncryptDecryptMsg() : Msg("EnableEncryptDecryptCmd", EnableEncryptDecryptCmd)
    {
    }

    EnableEncryptDecryptMsg::EnableEncryptDecryptMsg(MsgPacket& packet) : 
        Msg("EnableEncryptDecryptCmd", packet)
    {
    }

    EnableEncryptDecryptMsg::~EnableEncryptDecryptMsg()
    {
    }

    void EnableEncryptDecryptMsg::SerializeBody()
    {

        json j = *this;
        std::string jsonStr = j.dump(4); // 4-space indentation
        m_MsgPacket.SetMsgBodySize( jsonStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( jsonStr);

    }
    void EnableEncryptDecryptMsg::DeSerializeBody()
    {
        json bodyJson = json::parse(m_MsgPacket.GetBodyDataAsStr());
        m_nextIV = bodyJson["m_nextIV"].get<CryptoBlockVector>();
    }

    //////////////////////////////////////////////////////////////////////ValidateIVMsg/

    ValidateIVMsg::ValidateIVMsg() : Msg("ValidateIVCmd",  ValidateIVCmd)
    {
    }
    ValidateIVMsg::ValidateIVMsg(MsgPacket& packet) : 
        Msg("ValidateIVCmd", packet)
    {
    }

    ValidateIVMsg::~ValidateIVMsg()
    {
    }

    void ValidateIVMsg::SerializeBody()
    {
        json j = *this;
        std::string jsonStr = j.dump(4); // 4-space indentation
        m_MsgPacket.SetMsgBodySize( jsonStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( jsonStr);
    }

    void ValidateIVMsg::DeSerializeBody()
    {
        json jsonBody = json::parse(m_MsgPacket.GetBodyDataAsStr());
        IVToValidate = jsonBody["IVToValidate"].get<CryptoBlockVector>();
    }

    void ValidateIVMsg::SetIVToValidate(CryptoBlockVector iv)
    {
        IVToValidate = iv;
    }
    CryptoBlockVector ValidateIVMsg::GetIVToValidate()
    {
        return IVToValidate;
    }

    //////////////////////////////////////////////////////////////////////ValidateIVMsg/
    ValidateIVResultMsg::ValidateIVResultMsg() : Msg( "ValidateIVResultCmd",ValidateIVResultCmd)
    {
    }

    ValidateIVResultMsg::ValidateIVResultMsg(MsgPacket& packet) : 
        Msg("ValidateIVResultCmd", packet)
    {
    }

    ValidateIVResultMsg::~ValidateIVResultMsg()
    {
    }

    void ValidateIVResultMsg::DeSerializeBody()
    {

        json jsonBody = json::parse(m_MsgPacket.GetBodyDataAsStr());
        IsValid = jsonBody["IVValid"].get<bool>();
        
    }
    void ValidateIVResultMsg::SerializeBody()
    {
        json j = *this;
        std::string JSONStr = j.dump(4); // 4-space indentation
        m_MsgPacket.SetMsgBodySize( JSONStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( JSONStr);

    }

    void ValidateIVResultMsg::SetIsValid(bool isValid)
    {
        IsValid = isValid;
    }
    bool ValidateIVResultMsg::GetIsValid()
    {
        return IsValid;
    }   

    ///////////////////////////////////////////////////////////////////////////////////

    UpdateKeyAndIVMsg::UpdateKeyAndIVMsg() : Msg("UpdateKeyAndIVCmd", UpdateKeyAndIVCmd)
    {
        CreateNewKeyAndIV();
    }

    UpdateKeyAndIVMsg::UpdateKeyAndIVMsg(MsgPacket& packet) : 
        Msg("UpdateKeyAndIVCmd", packet)
    {
    }

    UpdateKeyAndIVMsg::~UpdateKeyAndIVMsg()
    {
    }

    void UpdateKeyAndIVMsg::SerializeBody()
    {
        json j = *this;
        std::string JSONStr = j.dump(4);
        m_MsgPacket.SetMsgBodySize( JSONStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( JSONStr);   
    }

    void UpdateKeyAndIVMsg::DeSerializeBody()
    {
        json jsonBody = json::parse(m_MsgPacket.GetBodyDataAsStr());
        m_IVAndKeyValues = jsonBody["m_IVAndKeyValues"].get<IVAndKeyValues>();


//        JSONDocumentWrapper docWrapper(*(new Document()));
//        docWrapper.Parse( m_MsgPacket.GetBodyDataAsStr());

//        std::string keyStr = docWrapper.GetString("Key");
//        std::string ivStr = docWrapper.GetString("IV");

//        m_IVAndKeyValues.key.Base64Decode(keyStr);
//        m_IVAndKeyValues.iv.Base64Decode(ivStr);
    }

    void UpdateKeyAndIVMsg::CreateNewKeyAndIV()
    {
        m_IVAndKeyValues = AESAccessManagement::Get()->CreateIVAndKey();
    }


    //////////////////////////////////////////////////////////////////////ValidateIVMsg/
    DebugQueryMsg::DebugQueryMsg() : Msg("DebugQueryCmd", DebugQueryCmd) 
    {
    }

    DebugQueryMsg::DebugQueryMsg(MsgPacket& packet) : 
        Msg("DebugQueryCmd", packet)
    {
        
    }

    DebugQueryMsg::~DebugQueryMsg()
    {
    }
    void DebugQueryMsg::SerializeBody()
    {
        json j = *this;
        std::string JSONStr = j.dump(4);
        m_MsgPacket.SetMsgBodySize( JSONStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( JSONStr);
    }
    void DebugQueryMsg::DeSerializeBody()
    {
        json jsonBody = json::parse(m_MsgPacket.GetBodyDataAsStr());
        QueryStr = jsonBody["QueryStr"].get<std::string>();

    }

    void DebugQueryMsg::SetQueryStr(std::string queryStr)
    {
        QueryStr = queryStr;
    }
    std::string DebugQueryMsg::GetQueryStr()
    {
        return QueryStr;
    }


    ////////////////////////////////////////////////////////////////////
    DebugResponseMsg::DebugResponseMsg() : Msg("DebugResponseCmd", DebugResponseCmd)
    {
    }   
    DebugResponseMsg::DebugResponseMsg(MsgPacket& packet) : 
        Msg("DebugResponseCmd", packet)
    {
    }
    DebugResponseMsg::~DebugResponseMsg()
    {
    }
    void DebugResponseMsg::SerializeBody()
    {
        json j = *this;
        std::string JSONStr = j.dump(4);
        m_MsgPacket.SetMsgBodySize( JSONStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( JSONStr);
    }
    void DebugResponseMsg::DeSerializeBody()
    {

        json jsonBody = json::parse(m_MsgPacket.GetBodyDataAsStr());
        ResponseStr = jsonBody["ResponseStr"].get<std::string>();

    }

    void DebugResponseMsg::SetResponseStr(std::string responseStr)
    {
        ResponseStr = responseStr;
    }
    std::string DebugResponseMsg::GetResponseStr()
    {
        return ResponseStr;
    }


    ////////////////////////////////////////////////////////////
    KeepAliveMsg::KeepAliveMsg() : Msg("KeepAlive", KeepAliveCmd)
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
    RemoteConnectionOkMsg::RemoteConnectionOkMsg() : Msg("RemoteConnectionOk", RemoteConnectionOkCmd)
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
    RequestKeyAndIVMsg::RequestKeyAndIVMsg() : Msg("RequestKeyAndIV", RequestKeyAndIVCmd)
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

