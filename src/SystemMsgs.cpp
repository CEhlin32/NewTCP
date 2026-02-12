#include <SystemMsgs.h>
#include <Msg.h>
#include <MsgCmds.h>
#include <EnumExtender.h>   
namespace newtcp

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
        std::string jsonStr = TCPMsgEnumManager::Get().Serialize();
        m_MsgPacket.SetMsgBodySize( jsonStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( jsonStr);
    }

    
   SupportedCmdInfoMsg::SupportedCmdInfoMsg() : Msg("AvailableCmdInfoCmd", AvailableCmdInfoCmd)
    {
    }
    SupportedCmdInfoMsg::SupportedCmdInfoMsg(MsgPacket& packet) : Msg("AvailableCmdInfoCmd", packet)
    {
    }
    SupportedCmdInfoMsg::~SupportedCmdInfoMsg()
    {
    }

    void SupportedCmdInfoMsg::SerializeBody()
    {
        std::string jsonStr = TCPMsgEnumManager::Get().Serialize();
        m_MsgPacket.SetMsgBodySize( jsonStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( jsonStr);
    }

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
        JSONDocumentWrapper docWrapper(*(new Document()));
        docWrapper.Parse(m_MsgPacket.GetBodyDataAsStr());
        m_InitAuth = docWrapper.GetString("AuthInitCode");
    }

    void AutherizationStartRequestMsg::SerializeBody()
    {
        JSONWriterWrapper writer;
        writer.StartObject();
        writer.String("AuthInitCode");
        writer.String(m_InitAuth.c_str());
        writer.EndObject();
        std::string JSONStr = writer.GetBufferString();
        m_MsgPacket.SetMsgBodySize( JSONStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( JSONStr);
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
        JSONWriterWrapper writer;
        writer.StartObject();
        writer.String("NextIV");
        std::string ivStr = m_nextIV.Base64Encode();
        writer.String(ivStr.c_str());
        writer.EndObject();
        std::string JSONStr = writer.GetBufferString();
        m_MsgPacket.SetMsgBodySize( JSONStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( JSONStr);
    }
    void EnableEncryptDecryptMsg::DeSerializeBody()
    {
        JSONDocumentWrapper docWrapper(*(new Document()));
        docWrapper.Parse(m_MsgPacket.GetBodyDataAsStr() );
        string nextIVStr = docWrapper.GetString("NextIV");
        m_nextIV.Base64Decode(nextIVStr);
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
        JSONWriterWrapper writer;
        writer.StartObject();
        writer.String("IVToValidate");
        std::string ivStr = IVToValidate.Base64Encode();
        writer.String(ivStr.c_str());
        writer.EndObject();
        std::string JSONStr = writer.GetBufferString();
        m_MsgPacket.SetMsgBodySize( JSONStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( JSONStr);
    }

    void ValidateIVMsg::DeSerializeBody()
    {
        JSONDocumentWrapper docWrapper(*(new Document()));
        docWrapper.Parse( m_MsgPacket.GetBodyDataAsStr());
        std::string ivStr = docWrapper.GetString("IVToValidate");
        IVToValidate.Base64Decode(ivStr);
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
        JSONDocumentWrapper docWrapper(*(new Document()));
        docWrapper.Parse( m_MsgPacket.GetBodyDataAsStr());
        IsValid = docWrapper.GetBool("IVValid");
        
    }
    void ValidateIVResultMsg::SerializeBody()
    {
        JSONWriterWrapper writer;
        writer.StartObject();
        writer.String("IVValid");
        writer.Bool(IsValid);
        writer.EndObject();
        std::string JSONStr = writer.GetBufferString();
        m_MsgPacket.SetMsgBodySize( JSONStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( JSONStr);

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
        JSONWriterWrapper writer;
        writer.StartObject();

        
        std::string keyStr(m_IVAndKeyValues.key.Base64Encode());
        std::string ivStr(m_IVAndKeyValues.iv.Base64Encode());

        writer.String("Key");
        writer.String(keyStr.c_str());
        writer.String("IV");
        writer.String(ivStr.c_str());
        writer.EndObject();

        std::string JSONStr = writer.GetBufferString();
        m_MsgPacket.SetMsgBodySize( JSONStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( JSONStr);   
    }

    void UpdateKeyAndIVMsg::DeSerializeBody()
    {
        JSONDocumentWrapper docWrapper(*(new Document()));
        docWrapper.Parse( m_MsgPacket.GetBodyDataAsStr());

        std::string keyStr = docWrapper.GetString("Key");
        std::string ivStr = docWrapper.GetString("IV");

        m_IVAndKeyValues.key.Base64Decode(keyStr);
        m_IVAndKeyValues.iv.Base64Decode(ivStr);
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
        JSONWriterWrapper writer;
        writer.StartObject();
        writer.String("QueryStr");
        writer.String(QueryStr.c_str());
        writer.EndObject();
        std::string JSONStr = writer.GetBufferString();
        m_MsgPacket.SetMsgBodySize( JSONStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( JSONStr);
    }
    void DebugQueryMsg::DeSerializeBody()
    {
        JSONDocumentWrapper docWrapper(*(new Document()));
        docWrapper.Parse( m_MsgPacket.GetBodyDataAsStr());
        QueryStr = docWrapper.GetString("QueryStr");
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
        JSONWriterWrapper writer;
        writer.StartObject();
        writer.String("ResponseStr");
        writer.String(ResponseStr.c_str());
        writer.EndObject();
        std::string JSONStr = writer.GetBufferString();
        m_MsgPacket.SetMsgBodySize( JSONStr.size());
        m_MsgPacket.SetIsEncrypted( false);
        m_MsgPacket.SetBodyDataFromStr( JSONStr);
    }
    void DebugResponseMsg::DeSerializeBody()
    {
        JSONDocumentWrapper docWrapper(*(new Document()));
        docWrapper.Parse( m_MsgPacket.GetBodyDataAsStr());
        ResponseStr = docWrapper.GetString("ResponseStr");
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

