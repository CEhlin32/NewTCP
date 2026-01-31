#include <NewMsgFactory.h>
#include <MsgCmds.h>
#include <Msg.h>
#include <NewSystemMsgs.h>

namespace newtcp
{
    std::vector<NewMsgFactory*> NewMsgFactory::m_Factories;

    NewMsgFactory::NewMsgFactory()
    {
    }

    NewMsgFactory::~NewMsgFactory()
    {
    }
    void NewMsgFactory::AddMessageFactory(NewMsgFactory* factory)
    {
        m_Factories.push_back(factory);
    }
    Msg* NewMsgFactory::CreateMessage(MsgPacket& packet)
    {
        for(int i = 0; i < m_Factories.size(); ++i)
        {
            Msg* msg = m_Factories[i]->CreateMsgInstance(packet);
            if(msg != nullptr)
            {
                return msg;
            }
        }   
        return nullptr;
    }
    SystemMsgFactory::SystemMsgFactory() : NewMsgFactory()
    {
    }
    SystemMsgFactory::~SystemMsgFactory()
    {
    }
    Msg* SystemMsgFactory::CreateMsgInstance(MsgPacket& packet)
    {
        switch(packet.GetMsgID())
        {
            case EnableEncryptDecryptCmd:
                return  new EnableEncryptDecryptMsg(packet);
            case RequestKeyAndIVCmd:
                return new RequestKeyAndIVMsg(packet);
            case UpdateKeyAndIVCmd:
                return new UpdateKeyAndIVMsg(packet); 
            case KeepAliveCmd:
                return new KeepAliveMsg(packet);
            case RemoteConnectionOkCmd:
                return new RemoteConnectionOkMsg(packet);
            case AutherizationStartRequestCmd:
                return new AutherizationStartRequestMsg(packet);
            case AutherizationReadyCmd:
                return new AutherizationReadyMsg(packet);
            case ValidateIVCmd:
                return new ValidateIVMsg(packet);
            case ValidateIVResultCmd:
                return new ValidateIVResultMsg(packet);
            case DebugQueryCmd:
                return new DebugQueryMsg(packet);
            case DebugResponseCmd:
                return new DebugResponseMsg(packet);
            
            default:
                break;
        }
        return nullptr;
    }
} // namespace newtcp

