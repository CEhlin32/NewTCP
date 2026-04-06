#include <TCPServerManager.h>
#include <TCPServer.h>
#include <DebReport.h>
#include <SystemMsgConstants.cs.h>
namespace CE::tcp
{

    TCPServerManager* TCPServerManager::Get()
    {
        if( theInstance == nullptr)
        {
            throw std::runtime_error("TCPServerManager not initialized");
        }
        return theInstance;
    }

    TCPServerManager::TCPServerManager(const std::string& controllerName) :
         SystemMsgEnumIDs("TCPSystemCommands", SharedSysMsgConstants::SystemCmdNames)
    {
        theInstance = this;
        baseID = CE::tcp::MsgManager::Get().AddEnumIDs(SystemMsgEnumIDs);


        TCPSystemMsgs::GetInstance(); // make sure it is initialized and registered as a MsgCreator
        m_ControllerName = controllerName;
        m_NumberOfConnections = 0;
        m_NumberOfLocalConnecions = 0;
        m_NumberOfRemoteConnecions = 0;
        m_MaxConnections = 5;
        m_Error = 0;

#ifdef OLD_CODE    
        // Default Ports
        m_remotePort = 51717;
        m_localPort = 51718;
        m_pairingPort = 51719;
        m_debugPort = 51720;
#endif
        m_pUDPServer = nullptr;
    }

    bool TCPServerManager::Init(int numberOfServers)
    {
        bool result = true;

        // Create the servers
        CreateServer(LOCAL_SERVER, 2);
        CreateServer(REMOTE_SERVER, 2);
        CreateServer(PAIRING_SERVER, 1);
        CreateServer(DEBUG_SERVER, 1);
        m_pUDPServer = new UDPServer(m_ControllerName);
        m_pUDPServer->Start();
        return result;
    }

void TCPServerManager::PrintCurrentServerStatus(LogOption& cat)
{

    TCPServerManager* pManager =TCPServerManager::Get();
    DebugReport report(cat, true, "****    TCPStatus    ****\n");
    report.SetIndentLevel(1);
    for( size_t n = 0; n < pManager->m_activeServers.size(); n++)
    {
        std::vector<std::string> statusStrs = pManager->m_activeServers[n]->GetServerStatusStrs();
        report.AddFormatedText("%s Connection Status:\n", pManager->m_activeServers[n]->GetServerName().c_str());
        for (const auto& statusStr : statusStrs)
        {
            report.AddFormatedText("\t%s\n", statusStr.c_str());
        }
    }

    report.PrintReport();
}


    TCPServer* TCPServerManager::CreateServer(TCPServerTypes type, int maxConnections)
    {
        TCPServer* pServer = nullptr;
        switch (type)
        {
        case PAIRING_SERVER:
        {
            pServer = new TCPServer(PAIRING_SERVER);
            pServer->StartServer(SharedSysMsgConstants::PAIRING_PORT, 1);
            break;
        }

        case DEBUG_SERVER:
        {
            pServer = new TCPServer(DEBUG_SERVER);
            pServer->StartServer(SharedSysMsgConstants::DEBUG_PORT,1);
            break;
        }

        case LOCAL_SERVER:
        {
            pServer = new TCPServer(LOCAL_SERVER);
            pServer->StartServer(SharedSysMsgConstants::LOCAL_PORT,2);
            break;
        }

        case REMOTE_SERVER:
        {
            pServer = new TCPServer(REMOTE_SERVER);
            pServer->StartServer(SharedSysMsgConstants::REMOTE_PORT,2);
            break;
        }

        default:
            break;
        }
        m_NumberOfConnections++;
        m_activeServers.push_back(pServer);

        return pServer;
    }

    bool TCPServerManager::SendTo(Msg& msg, int connectionID)
    {
        bool result = false;

        for (auto pTCPConnection: TCPConnection::m_ActiveConnections)
        {
            if( pTCPConnection->GetSocketFd() != connectionID)
                continue;
            // Send to the connection with the specified ID
            result |= pTCPConnection->Send(msg);
        }      
    return result;
    }

    bool TCPServerManager::SendToAllConnected(Msg& msg)
    {
        bool result = false;

        for (auto pTCPConnection: TCPConnection::m_ActiveConnections)
        {
            // Send to all connected connections
            result |= pTCPConnection->Send(msg);
        }      
    return result;
    }


} // namespace CE::tcp