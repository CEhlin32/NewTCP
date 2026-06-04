#ifndef TCP_SERVER_MANAGER_H
#define TCP_SERVER_MANAGER_H

#include <string>
#include <vector>
#include <TCPServer.h>
#include <UDPClient.h>
#include <CommonDefinitions.h>
#include <Logger.h>


namespace CE::tcp
{

    class TCPServerManager
    {
    public:
        TCPServerManager(const std::string& controllerName);
        static TCPServerManager* Get(); 
        bool Init(int numberOfServers);
        TCPServer* CreateServer(TCPServerTypes type, int maxConnections = 5);
        bool IsServerConnected(TCPServerTypes type);
        int GetServerID(TCPServerTypes type);
        bool SendTo(Msg& msg, int connectionID);
        bool SendToAllConnected(Msg& msg);
        static void PrintCurrentServerStatus(LogOption& cat);
        virtual ~TCPServerManager(){;}
    private:

        inline static TCPServerManager* theInstance;
        int baseID;
        std::string m_ControllerName;
        std::vector<CE::tcp::TCPServer*> m_activeServers;

        int m_MaxConnections;
        int m_NumberOfLocalConnecions;
        int m_NumberOfRemoteConnecions;
        int m_NumberOfConnections;
        int m_Error;

        UDPServer* m_pUDPServer;
    };

} // namespace CE::tcp


#endif // TCP_SERVER_MANAGER_H