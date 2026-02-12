#include <TCPClient.h>

namespace newtcp
{

    TCPClient::TCPClient()  {}

    // Establish connection to a remote server
    bool TCPClient::Start(const std::string& address, int port) {
        int sock = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket
        if (sock == -1) return false;

        server.sin_addr.s_addr = inet_addr(address.c_str());
        server.sin_family = AF_INET;
        server.sin_port = htons(port);

        if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
            return false;
        }
        SetSocketFd(sock);
        StartReadThread();
        return true;
    }


    void TCPClient::Stop() 
    {
        StopReadThread();
    }

    TCPClient::~TCPClient() 
    {
         Stop(); 
    }
}