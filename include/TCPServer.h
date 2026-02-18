#ifndef NEW_TCPSERVER_H
#define NEW_TCPSERVER_H

#include <TCPConnection.h>
#include <CommonDefinitions.h>

#include <arpa/inet.h>

namespace CE::tcp
{
class TCPServer 
{
public:
    TCPServer(TCPServerTypes type);
    ~TCPServer();
    bool StartServer(int port, int maxConnections = 5);
    void StopServer();  
    const TCPServerTypes GetServerType() const;
    bool SendToAll(Msg& msg);
    bool Send(Msg& msg);
    const std::string GetServerName() const;
    const std::vector<std::string> GetServerStatusStrs() const;

private:
    NamedThread* m_pAcceptThread;
    std::thread::id m_AcceptThreadId;
    void AcceptThread();

    std::vector<TCPConnection*> m_tcpConnections;
    sockaddr_in m_server_addr;
    int m_server_socket_fd;
    TCPServerTypes m_serverType;
};

}


#endif // NEW_TCPSERVER_H