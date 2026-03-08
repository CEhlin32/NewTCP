#ifndef NEW_SYSTEM_MSGS_H
#define NEW_SYSTEM_MSGS_H

#include <Msg.h>
#include <JSONValueWrapper.h>
#include <CryptoKeyIV.h>
#include <AESSupport.h>
#include <EnumExtender.h>
#include <MsgCmds.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json_abi_v3_12_0::json;

namespace CE::tcp
{
    class TCPMsgEnumManager : public EnumExtenderManager
    {
    public:
        static TCPMsgEnumManager& Get()
        {
            static TCPMsgEnumManager instance;
            return instance;
        }

        TCPMsgEnumManager() : EnumExtenderManager("TCPMsgEnumManager")
        {
            AddEnumExtender("TCPMsgCommands", MsgCmds::SystemCmdNames);
        }
    };


    class AvailableCmdInfoMsg : public Msg
    {
    public:
        AvailableCmdInfoMsg();
        AvailableCmdInfoMsg(MsgPacket& packet);
        virtual ~AvailableCmdInfoMsg();

    protected:    
        void SerializeBody() override ;
        void DeSerializeBody() override ;
    private:
        std::vector<std::string> m_AvailableCmdNames;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(AvailableCmdInfoMsg, m_AvailableCmdNames)
    };    

class AutherizationStartRequestMsg : public Msg
{
    public:

        AutherizationStartRequestMsg();
        AutherizationStartRequestMsg(MsgPacket& packet);

        virtual ~AutherizationStartRequestMsg();

        std::string GetInitValue()
        {
            return m_InitAuth;
        }

    protected:    
        void SerializeBody() override ;
        void DeSerializeBody() override;


    private:
        std::string m_InitAuth;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(AutherizationStartRequestMsg, m_InitAuth)
        
};

class AutherizationReadyMsg : public Msg
{
    public:

        AutherizationReadyMsg();
        AutherizationReadyMsg(MsgPacket& packet);   
        virtual ~AutherizationReadyMsg();


};

class EnableEncryptDecryptMsg: public Msg
{
    public:
        EnableEncryptDecryptMsg();
        EnableEncryptDecryptMsg(MsgPacket& packet);
        virtual ~EnableEncryptDecryptMsg();

        CryptoBlockVector GetEncryptionIV()
        {
            if( true == m_MsgPacket.GetIsEncrypted())
                return m_nextIV;

            return defaultIV;
        }
        
        CryptoBlockVector m_nextIV;

    protected:    
        void SerializeBody() override ;
        void DeSerializeBody() override ;

    private:
    inline static CryptoBlockVector defaultIV;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(EnableEncryptDecryptMsg, m_nextIV)
};

class RequestKeyAndIVMsg: public Msg
{
public:
    RequestKeyAndIVMsg();
    RequestKeyAndIVMsg(MsgPacket& packet);
    virtual ~RequestKeyAndIVMsg();


};

class KeepAliveMsg : public Msg
{
public:
    KeepAliveMsg();
    KeepAliveMsg(MsgPacket& packet);
    virtual ~KeepAliveMsg();


};

class RemoteConnectionOkMsg : public Msg
{
public:
    RemoteConnectionOkMsg();
    RemoteConnectionOkMsg(MsgPacket& packet);
    virtual ~RemoteConnectionOkMsg();

    
};

class UpdateKeyAndIVMsg : public Msg
{
public:
    UpdateKeyAndIVMsg();
    UpdateKeyAndIVMsg(MsgPacket& packet);
    virtual ~UpdateKeyAndIVMsg();

    void CreateNewKeyAndIV();
protected:    
    void SerializeBody() override ;
    void DeSerializeBody() override ;
    IVAndKeyValues m_IVAndKeyValues;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UpdateKeyAndIVMsg, m_IVAndKeyValues)
};
class ValidateIVMsg :  public Msg
{
public:
    ValidateIVMsg();
    ValidateIVMsg(MsgPacket& packet);
    virtual ~ValidateIVMsg();
    void SetIVToValidate(CryptoBlockVector iv);
    CryptoBlockVector GetIVToValidate();

protected:    
    void SerializeBody() override ;
    void DeSerializeBody() override ;

    CryptoBlockVector IVToValidate;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ValidateIVMsg, IVToValidate)
};

class ValidateIVResultMsg : public Msg
{
public:
    ValidateIVResultMsg();
    ValidateIVResultMsg(MsgPacket& packet);
    virtual ~ValidateIVResultMsg();
    void SetIsValid(bool isValid);
    bool GetIsValid();
protected:    
    void DeSerializeBody() override ;
    void SerializeBody() override ;

    bool IsValid;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ValidateIVResultMsg, IsValid)
};



class DebugQueryMsg : public Msg
{
public:
    DebugQueryMsg();
    DebugQueryMsg(MsgPacket& packet);
    virtual ~DebugQueryMsg();

    void SetQueryStr(std::string queryStr);
    std::string GetQueryStr();

protected:    
    void SerializeBody() override;
    void DeSerializeBody() override ;
    std::string QueryStr;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DebugQueryMsg, QueryStr)
};

class DebugResponseMsg : public Msg
{
public:
    DebugResponseMsg();
    DebugResponseMsg(MsgPacket& packet);

    virtual ~DebugResponseMsg();

    void SetResponseStr(std::string responseStr);
    std::string GetResponseStr();

protected:    
    void SerializeBody() override;
    void DeSerializeBody() override ;
    std::string ResponseStr; 
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DebugResponseMsg, ResponseStr)
};
}
#endif // NEW_SYSTEM_MSGS_H