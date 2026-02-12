#include <TCPServerManager.h>
#include <TCPServer.h>
#include <DebReport.h>

namespace newtcp
{

    TCPServerManager* TCPServerManager::Get()
    {
        if( theInstance == nullptr)
        {
            throw std::runtime_error("TCPServerManager not initialized");
        }
        return theInstance;
    }

    TCPServerManager::TCPServerManager(const std::string& controllerName)
    {
        theInstance = this;
        m_ControllerName = controllerName;
        m_NumberOfConnections = 0;
        m_NumberOfLocalConnecions = 0;
        m_NumberOfRemoteConnecions = 0;
        m_MaxConnections = 5;
        m_Error = 0;

        // Default Ports
        m_remotePort = 51717;
        m_localPort = 51718;
        m_pairingPort = 51719;
        m_debugPort = 51720;

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
            pServer->StartServer(m_pairingPort, 1);
            break;
        }

        case DEBUG_SERVER:
        {
            pServer = new TCPServer(DEBUG_SERVER);
            pServer->StartServer(m_debugPort,1);
            break;
        }

        case LOCAL_SERVER:
        {
            pServer = new TCPServer(LOCAL_SERVER);
            pServer->StartServer(m_localPort,2);
            break;
        }

        case REMOTE_SERVER:
        {
            pServer = new TCPServer(REMOTE_SERVER);
            pServer->StartServer(m_remotePort,2);
            break;
        }

        default:
            break;
        }
        m_NumberOfConnections++;
        m_activeServers.push_back(pServer);

        return pServer;
    }

    bool TCPServerManager::SendTo(Msg& msg, TCPServerTypes types)
    {
        bool result = false;

        for (auto pServer : m_activeServers)
        {
            if( (pServer->GetServerType() & types) == 0)
                continue;
            // Send to all servers of the specified type  
            result |= pServer->Send(msg);
        }      
    return result;
    }

} // namespace newtcp