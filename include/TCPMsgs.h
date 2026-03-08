#ifndef TCP_MSGS_H
#define TCP_MSGS_H

#include <vector>
#include <string>
#include <EnumIDs.h>
#include <EnumMsgIDMgr.h>
#include <CryptoKeyIV.h>

namespace Test
{
    class TCPMsgs : public IMsgCreator
    {
    public:
        static TCPMsgs& GetInstance();
        enum TCPMsgCommands
        {
            AvailableCmdInfoCmd = 0,
            EnableEncryptDecryptCmd,
            RequestKeyAndIVCmd,
            UpdateKeyAndIVCmd,
            AutherizationStartRequestCmd,
            AutherizationReadyCmd,
            AutherizationValidationCmd,
            AutherizationValidationResponseCmd,
            ValidateIVCmd,
            ValidateIVResultCmd,
            KeepAliveCmd,
            RemoteConnectionOkCmd,
            DebugQueryCmd,
            DebugResponseCmd
        };

        EnumIDs GetTCPMsgEnumIDs();
    private:
            TCPMsgs();
            TCPMsgs(const TCPMsgs&) = delete;
            TCPMsgs& operator=(const TCPMsgs&) = delete;
            CE::tcp::Msg* CreateMsg(CE::tcp::MsgPacket& packet) override;

            const std::string TCPMsgCommandsName;
            const std::vector<std::string> TCPCmdNames;
            EnumIDs TCPMsgEnumIDs;
            int baseID = 0;
    };
/////////////////////////////////////////////////////////////////////////////

    class AvailableCmdInfoMsg : public CE::tcp::Msg
    {
    public:
        AvailableCmdInfoMsg();
        AvailableCmdInfoMsg(CE::tcp::MsgPacket& packet);
        virtual ~AvailableCmdInfoMsg();

    protected:    
        void SerializeBody() override;

        void DeSerializeBody() override ;

    private:
        std::vector<std::string> m_AvailableCmdNames;

    };    



//////////////////////////////////////////////////////////////////////////

class AutherizationStartRequestMsg : public CE::tcp::Msg
{
    public:
 
        AutherizationStartRequestMsg();
        AutherizationStartRequestMsg(CE::tcp::MsgPacket& packet);


        virtual ~AutherizationStartRequestMsg();

        std::string GetInitValue();

    protected:    
        void SerializeBody() override; 
        void DeSerializeBody() override;


    private:
        std::string m_InitAuth;


};


class AutherizationReadyMsg : public CE::tcp::Msg
{
    public:

        AutherizationReadyMsg();
        AutherizationReadyMsg(CE::tcp::MsgPacket& packet);   
        virtual ~AutherizationReadyMsg();


};

class EnableEncryptDecryptMsg: public CE::tcp::Msg
{
    public:
        EnableEncryptDecryptMsg();
        EnableEncryptDecryptMsg(CE::tcp::MsgPacket& packet);
        virtual ~EnableEncryptDecryptMsg();

        CryptoBlockVector GetEncryptionIV();
        
        CryptoBlockVector m_nextIV;

    protected:    
        void SerializeBody() override ;
        void DeSerializeBody() override ;

    private:
    inline static CryptoBlockVector defaultIV;
};

class RequestKeyAndIVMsg: public CE::tcp::Msg
{
public:
    RequestKeyAndIVMsg();
    RequestKeyAndIVMsg(CE::tcp::MsgPacket& packet);
    virtual ~RequestKeyAndIVMsg();


};

class KeepAliveMsg : public CE::tcp::Msg
{
public:
    KeepAliveMsg();
    KeepAliveMsg(CE::tcp::MsgPacket& packet);
    virtual ~KeepAliveMsg();


};

class RemoteConnectionOkMsg : public CE::tcp::Msg
{
public:
    RemoteConnectionOkMsg();
    RemoteConnectionOkMsg(CE::tcp::MsgPacket& packet);
    virtual ~RemoteConnectionOkMsg();

    
};

class UpdateKeyAndIVMsg : public CE::tcp::Msg
{
public:
    UpdateKeyAndIVMsg();
    UpdateKeyAndIVMsg(CE::tcp::MsgPacket& packet);
    virtual ~UpdateKeyAndIVMsg();

    void CreateNewKeyAndIV();
protected:    
    void SerializeBody() override ;
    void DeSerializeBody() override ;
    IVAndKeyValues m_IVAndKeyValues;
};

class ValidateIVMsg :  public CE::tcp::Msg
{
public:
    ValidateIVMsg();
    ValidateIVMsg(CE::tcp::MsgPacket& packet);
    virtual ~ValidateIVMsg();
    void SetIVToValidate(CryptoBlockVector iv);
    CryptoBlockVector GetIVToValidate();

protected:    
    void SerializeBody() override ;
    void DeSerializeBody() override ;

    CryptoBlockVector IVToValidate;
};

class ValidateIVResultMsg : public CE::tcp::Msg
{
public:
    ValidateIVResultMsg();
    ValidateIVResultMsg(CE::tcp::MsgPacket& packet);
    virtual ~ValidateIVResultMsg();
    void SetIsValid(bool isValid);
    bool GetIsValid();
protected:    
    void DeSerializeBody() override ;
    void SerializeBody() override ;

    bool IsValid;
};



class DebugQueryMsg : public CE::tcp::Msg
{
public:
    DebugQueryMsg();
    DebugQueryMsg(CE::tcp::MsgPacket& packet);
    virtual ~DebugQueryMsg();

    void SetQueryStr(std::string queryStr);
    std::string GetQueryStr();

protected:    
    void SerializeBody() override;
    void DeSerializeBody() override ;
    std::string QueryStr;
};

class DebugResponseMsg : public CE::tcp::Msg
{
public:
    DebugResponseMsg();
    DebugResponseMsg(CE::tcp::MsgPacket& packet);

    virtual ~DebugResponseMsg();

    void SetResponseStr(std::string responseStr);
    std::string GetResponseStr();

protected:    
    void SerializeBody() override;
    void DeSerializeBody() override ;
    std::string ResponseStr; 
    
};



} // namespace Test

#endif // TCP_MSGS_H