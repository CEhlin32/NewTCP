




#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <UDPClient.h>
#include <ExternalIPAddr.h>
#include <Logger.h>
#include <JSONValueWrapper.h>


#define PORT     8080 
#define MAXLINE 1024 

namespace CE::tcp
{
        ControllerData::ControllerData(const std::string& name, const std::string& ipAddr) :
            ControllerName(name),
            ControllerIPAddrStr(ipAddr)
        {
        }
        std::string ControllerData::Serialize()
        {
            std::string result = "";
            JSONWriterWrapper writer;

            try
            {
                writer.StartObject();

                writer.String("ControllerName");
                writer.String(ControllerName.c_str());

                writer.String("IPAddress");
                writer.String(ControllerIPAddrStr.c_str());
                writer.EndObject();
                result = writer.GetBufferString();
            }
            catch (JSONException &e)
            {
            }
            return result;
        }



    UDPServer::UDPServer(const std::string& controllerName) : 
        sockfd(-1),
        m_pUDPThread(nullptr),
        m_ControllerName(controllerName),
        m_KeepAlive(true)
    {
    }

    UDPServer::~UDPServer()
    {
        
    }



    int  GetIPAddress()
    {
        
        int sock = socket(PF_INET, SOCK_DGRAM, 0);
        if (sock == -1) {
            std::cerr << "Socket creation failed" << std::endl;
            return 1;
        }

        sockaddr_in loopback;
        std::memset(&loopback, 0, sizeof(loopback));
        loopback.sin_family = AF_INET;
        loopback.sin_addr.s_addr = INADDR_LOOPBACK; 
        loopback.sin_port = htons(9);

        if (connect(sock, reinterpret_cast<sockaddr*>(&loopback), sizeof(loopback)) == -1) {
            close(sock);
            std::cerr << "Connect failed" << std::endl;
            return 1;
        }

        socklen_t addrlen = sizeof(loopback);
        if (getsockname(sock, reinterpret_cast<sockaddr*>(&loopback), &addrlen) == -1) {
            close(sock);
            std::cerr << "getsockname failed" << std::endl;
            return 1;
        }

        char buf[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &loopback.sin_addr, buf, INET_ADDRSTRLEN) == nullptr) {
            close(sock);
            std::cerr << "inet_ntop failed" << std::endl;
            return 1;
        }

        std::cout << "Local IP address: " << buf << std::endl;
        close(sock);
        return 0;   
    }
    int UDPServer::UDPServerThread() 
    { 
        char buffer[MAXLINE]; 
        struct sockaddr_in servaddr, cliaddr; 
        GetIPAddress();     

        
        // Creating socket file descriptor 
        if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
            perror("socket creation failed"); 
            exit(EXIT_FAILURE); 
        } 
        
        memset(&servaddr, 0, sizeof(servaddr)); 
        memset(&cliaddr, 0, sizeof(cliaddr)); 
        
        // Filling server information 
        servaddr.sin_family    = AF_INET; // IPv4 
        servaddr.sin_addr.s_addr = INADDR_ANY; 
        servaddr.sin_port = htons(PORT); 
        
        // Bind the socket with the server address 
        if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
                sizeof(servaddr)) < 0 ) 
        { 
            perror("bind failed"); 
            exit(EXIT_FAILURE); 
        } 
        
        socklen_t len;
        int n;  
        m_KeepAlive = true;
        len = sizeof(cliaddr);  //len is value/result 
        while(m_KeepAlive)
        {
            n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                        MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                        &len); 
            buffer[n] = '\0'; 
            TheAppLogger.LogMsgWithTime(DebugInfoLogOption::instance(),"Client : %s\n", buffer); 
            std::string requestStr(buffer);
            if( (std::string::npos != requestStr.find("Discover SprinklerController") ) ||
                (std::string::npos != requestStr.find("Discover All") ))
            {
                cout << "Received discovery request: " << requestStr << endl;
                std::string ipAddrStr = ExternalIPAddr::GetExternalIP();
                ControllerData ctrlData("Sprinkler", ipAddrStr);
                std::string serailizedData = ctrlData.Serialize();

                int dataLen = serailizedData.length();
                printf("Sending UDP response to discovery request. DataLen=%d Data=%s\n", 
                    dataLen, serailizedData.c_str());
                sendto(sockfd, serailizedData.c_str(), serailizedData.length(),  
                    MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
                        len); 
            }
            else
            {
                cout << "Received unknown UDP request: " << requestStr << endl;
            }
        }
        
        return 0; 
    }

    void UDPServer::Start()
    {
        m_pUDPThread = new NamedThread("UDP_ServerThread", &UDPServer::UDPServerThread, this);


    }

    void UDPServer::Stop()
    {
        close(sockfd);
        m_KeepAlive = false;
    }
} // namespace CE::tcp

