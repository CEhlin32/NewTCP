#ifndef NEW_TCPCLIENT_H
#define NEW_TCPCLIENT_H
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <TCPConnection.h>

namespace CE::tcp
{

    class TCPClient : public TCPConnection
    {
    private:
        struct sockaddr_in server;

    public:
        TCPClient();

        // Establish connection to a remote server
        bool Start(const std::string& address, int port);
        

        void Stop(); 

        ~TCPClient();
    };
}


#endif // NEW_TCPCLIENT_H