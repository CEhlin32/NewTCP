#include <TCPServer.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

namespace newtcp
{


    TCPServer::TCPServer(TCPServerTypes type) : m_serverType(type)
    {
    }

    TCPServer::~TCPServer()
    {
        StopServer();
    }

    const std::string TCPServer::GetServerName() const
    {
        switch (m_serverType)
        {
            case LOCAL_SERVER:
                return "Local TCP Server";
            case REMOTE_SERVER:
                return "Remote TCP Server";
            case PAIRING_SERVER:
                return "Pairing TCP Server";
            case DEBUG_SERVER:
                return "Debug TCP Server";
            default:
                return "Unknown TCP Server";
        }
    }
    const std::vector<std::string> TCPServer::GetServerStatusStrs() const
    {
        std::vector<std::string> statusStrs;
        for (const auto& connection : m_tcpConnections)
        {
            statusStrs.push_back(TCPConnectionStatus::ServerStatusStr[connection->GetConnectionStatus()]);
        }
        return statusStrs;
    }

    bool TCPServer::StartServer(int port, int maxConnections)
    {
        for(int i = 0; i < maxConnections; i++)
        {   
            TCPConnection* connection = new TCPConnection();
            m_tcpConnections.push_back(connection);
        }

        m_server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (m_server_socket_fd < 0)
        {
            std::cerr << "Failed to create socket" << std::endl;
            return false;
        }

        std::memset(&m_server_addr, 0, sizeof(m_server_addr));
        m_server_addr.sin_family = AF_INET;
        m_server_addr.sin_addr.s_addr = INADDR_ANY;
        m_server_addr.sin_port = htons(port);

        if (bind(m_server_socket_fd, (struct sockaddr*)&m_server_addr, sizeof(m_server_addr)) < 0)
        {
            std::cerr << "Bind failed" << std::endl;
            close(m_server_socket_fd);
            return false;
        }

        if (listen(m_server_socket_fd, maxConnections) < 0)
        {
            std::cerr << "Listen failed" << std::endl;
            close(m_server_socket_fd);
            return false;
        }
        m_pAcceptThread = new NamedThread("TCPServerAcceptThread",
            &TCPServer::AcceptThread, this);
        m_AcceptThreadId = m_pAcceptThread->get_id();
        return true;
        }

    void TCPServer::AcceptThread()
    {
        int addrlen = sizeof(m_server_addr);
        while(true)
        {
            int client_socket = accept(m_server_socket_fd, (struct sockaddr *)&m_server_addr, (socklen_t*)&addrlen);
            if (client_socket < 0)
            {
                std::cerr << "Accept failed" << std::endl;
                close(m_server_socket_fd);
                continue;
            }
            for (auto conn : m_tcpConnections)
            {
                TCPConnection tcpConnection = *conn;
                if( tcpConnection.GetConnectionStatus() == newtcp::TCPConnectionStatus::Connection_Available)
                {
                    conn->SetSocketFd(client_socket);
                    conn->StartReadThread();
                    break;
                }
            }
        }
    }

    void TCPServer::StopServer()
    {
        for (auto connection : m_tcpConnections)
        {
            connection->StopReadThread();
            delete connection;
        }
        m_tcpConnections.clear();
        close(m_server_socket_fd);
        m_server_socket_fd = -1;
    }

    const TCPServerTypes TCPServer::GetServerType() const 
    { 
        return m_serverType; 
    }
    
    /// @brief Send a message to all active connections on this server
    /// @param msg The message to send
    /// @return True if the message was sent to all connections successfully, false otherwise
    bool TCPServer::SendToAll(Msg& msg)
    {
        bool result = true;
        for (auto connection : m_tcpConnections)
        {
            if (!connection->Send(msg))
            {
                result = false;
            }
        }
        return result;

    }

    /// @brief Send a message to a specific connection by connection ID
    /// @param msg The message to send
    /// @return True if the message was sent successfully, false otherwise
    bool TCPServer::Send(Msg& msg)
    {
        bool result = false;
        for (auto connection : m_tcpConnections)
        {
            if (connection->GetSocketFd() == msg.GetConnectionID())
            {
                if (0 == connection->Send(msg))   
                {
                    result = true;
                }
                break;
            }
        }
        return result;
    }

} // namespace newtcp