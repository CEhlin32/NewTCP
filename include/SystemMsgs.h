#ifndef NEW_SYSTEM_MSGS_H
#define NEW_SYSTEM_MSGS_H

#include <Msg.h>
#include <JSONValueWrapper.h>
#include <CryptoKeyIV.h>
#include <AESSupport.h>
#include <EnumExtender.h>
#include <EnumIDs.h>

#include <nlohmann/json.hpp>
#include <MsgProcessor.h>
#include <SystemMsgConstants.cs.h>

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
            AddEnumExtender("TCPSystemCommands", SharedSysMsgConstants::SystemCmdNames);
        }
    };

    class TCPSystemMsgs : public CE::tcp::ICreateMsgFromPacket
    {
    public:
            static TCPSystemMsgs& GetInstance()
            {
                static TCPSystemMsgs instance;
                return instance;
            }

        EnumIDs GetTCPSystemMsgEnumIDs();
    private:
            TCPSystemMsgs();
            TCPSystemMsgs(const TCPSystemMsgs&) = delete;
            TCPSystemMsgs& operator=(const TCPSystemMsgs&) = delete;
            Msg* CreateMsg(CE::tcp::MsgPacket& packet) override;
        const std::string TCPSystemMsgCommandsName;
        EnumIDs TCPSystemMsgEnumIDs;
        int baseID = 0;
    };


#ifdef OLD_CDOE
    class SystemMsgCreator : public ICreateMsgFromPacket
    {
        public:
            static SystemMsgCreator& GetInstance()
            {
                static SystemMsgCreator instance;
                return instance;
            }
        private:    
            SystemMsgCreator();
            Msg* CreateMsg(MsgPacket& packet) override;
    };

#endif

    class TestMsg : public Msg
    {
    public:
        TestMsg();
        TestMsg(MsgPacket& packet);
        virtual ~TestMsg();
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
            return CLIENT_AUTH_ID;
        }

    protected:    
        void SerializeBody() override ;
        void DeSerializeBody() override;


    private:
        std::string CLIENT_AUTH_ID;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(AutherizationStartRequestMsg, CLIENT_AUTH_ID)
        
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

#if OLD_CODE        
        CryptoBlockVector GetNextIV()
        {
            return NEXT_IV;
        }
        CryptoBlockVector GetNewKey()
        {
            return newKey;
        }
        
        CryptoBlockVector NEXT_IV;

    protected:    
        void SerializeBody() override ;
        void DeSerializeBody() override ;

    private:
    CryptoBlockVector newKey;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(EnableEncryptDecryptMsg, NEXT_IV)
#endif
};

class AutherizationValidationMsg : public Msg
{
public:
    AutherizationValidationMsg();
    AutherizationValidationMsg(MsgPacket& packet);
    virtual ~AutherizationValidationMsg();

    void SerializeBody() override ;
    void DeSerializeBody() override ;

    CryptoBlockVector IV_TO_VALIDATE;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AutherizationValidationMsg, IV_TO_VALIDATE)
};
class AutherizationValidationResponseMsg : public Msg
{
public:
    AutherizationValidationResponseMsg();
    AutherizationValidationResponseMsg(MsgPacket& packet);
    virtual ~AutherizationValidationResponseMsg();

    void SerializeBody() override ;
    void DeSerializeBody() override ;

    bool IV_IS_VALID;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AutherizationValidationResponseMsg, IV_IS_VALID)
};

class AutherizationEndMsg : public Msg
{
public:
    AutherizationEndMsg();
    AutherizationEndMsg(MsgPacket& packet);
    virtual ~AutherizationEndMsg();
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

    CryptoBlockVector GetIV();
    CryptoBlockVector GetKey();

    void CreateNewKeyAndIV();
protected:    
    void SerializeBody() override ;
    void DeSerializeBody() override ;
    IVAndKeyValues IV_AND_KEY;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UpdateKeyAndIVMsg, IV_AND_KEY)
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

    CryptoBlockVector IV_TO_VALIDATE;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ValidateIVMsg, IV_TO_VALIDATE)
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

    bool IV_IS_VALID;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ValidateIVResultMsg, IV_IS_VALID)
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
    std::string QUERY_STR;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DebugQueryMsg, QUERY_STR)
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
    std::string RESPONSE_STR; 
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DebugResponseMsg, RESPONSE_STR)
};
}
#endif // NEW_SYSTEM_MSGS_H