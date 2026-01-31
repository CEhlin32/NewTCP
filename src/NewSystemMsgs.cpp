#include <NewSystemMsgs.h>
#include <Msg.h>
#include <MsgCmds.h>

namespace newtcp
{

AutherizationStartRequestMsg::AutherizationStartRequestMsg() : 
    Msg("AutherizationStartRequest", AutherizationStartRequestCmd)
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

AutherizationReadyMsg::AutherizationReadyMsg() : Msg("AutherizationReady", AutherizationReadyCmd)
{

}

AutherizationReadyMsg::AutherizationReadyMsg(MsgPacket& packet) : Msg("AutherizationReady", packet)
{
}

AutherizationReadyMsg::~AutherizationReadyMsg()
{

}
////////////////////////////////////////////////////////////////

EnableEncryptDecryptMsg::EnableEncryptDecryptMsg() : Msg("EnableEncryptDecrypt", EnableEncryptDecryptCmd)
{
}

EnableEncryptDecryptMsg::EnableEncryptDecryptMsg(MsgPacket& packet) : 
    Msg("EnableEncryptDecrypt", packet)
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

ValidateIVMsg::ValidateIVMsg() : Msg("ValidateIV",  ValidateIVCmd)
{
}
ValidateIVMsg::ValidateIVMsg(MsgPacket& packet) : 
    Msg("ValidateIV", packet)
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
ValidateIVResultMsg::ValidateIVResultMsg() : Msg( "ValidateIVResult",ValidateIVResultCmd)
{
}

ValidateIVResultMsg::ValidateIVResultMsg(MsgPacket& packet) : 
    Msg("ValidateIVResult", packet)
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

UpdateKeyAndIVMsg::UpdateKeyAndIVMsg() : Msg("UpdateKeyAndIV", UpdateKeyAndIVCmd)
{
    CreateNewKeyAndIV();
}

UpdateKeyAndIVMsg::UpdateKeyAndIVMsg(MsgPacket& packet) : 
    Msg("UpdateKeyAndIV", packet)
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


void UpdateKeyAndIVMsg::CreateNewKeyAndIV()
{
    m_IVAndKeyValues = AESAccessManagement::Get()->CreateIVAndKey();
}


//////////////////////////////////////////////////////////////////////ValidateIVMsg/
DebugQueryMsg::DebugQueryMsg() : Msg("DebugQuery", DebugQueryCmd) 
{
}

DebugQueryMsg::DebugQueryMsg(MsgPacket& packet) : 
    Msg("DebugQuery", packet)
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

}