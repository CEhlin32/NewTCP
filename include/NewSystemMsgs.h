#ifndef NEW_SYSTEM_MSGS_H
#define NEW_SYSTEM_MSGS_H

#include <Msg.h>
#include <JSONValueWrapper.h>
#include <CryptoKeyIV.h>
#include <AESSupport.h>

namespace newtcp
{

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
    static CryptoBlockVector defaultIV;

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
};

class ValidateIVMsg :  public Msg
{
public:
    ValidateIVMsg();
    ValidateIVMsg(MsgPacket& packet);
    virtual ~ValidateIVMsg();

protected:    
    void SerializeBody() override ;
    void DeSerializeBody() override ;

    CryptoBlockVector IVToValidate;
};

class ValidateIVResultMsg : public Msg
{
public:
    ValidateIVResultMsg();
    ValidateIVResultMsg(MsgPacket& packet);
    virtual ~ValidateIVResultMsg();
protected:    
    void DeSerializeBody() override ;
    void SerializeBody() override ;

    bool IsValid;
};



class DebugQueryMsg : public Msg
{
public:
    DebugQueryMsg();
    DebugQueryMsg(MsgPacket& packet);
    virtual ~DebugQueryMsg();

protected:    
    void SerializeBody() override;
    void DeSerializeBody() override ;
    std::string QueryStr;
};

class DebugResponseMsg : public Msg
{
public:
    DebugResponseMsg();
    DebugResponseMsg(MsgPacket& packet);

protected:    
    void SerializeBody() override;
    void DeSerializeBody() override ;
    std::string ResponseStr; 
};
}
#endif // NEW_SYSTEM_MSGS_H