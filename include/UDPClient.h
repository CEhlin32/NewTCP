#ifndef UDPClient_H
#define UDPClient_H

#include <Thread.h>

namespace CE::tcp
{
    class ControllerData
    {
        public:
        std::string ControllerName;
        std::string ControllerIPAddrStr;

        ControllerData(const std::string& Name, const std::string& IpAddr);
        std::string Serialize();
    };

    class UDPServer
    {
        public:
        explicit UDPServer(const std::string& controllerName);
        virtual ~UDPServer();

        void Start();
        void Stop();



        private:
        int UDPServerThread();
        int localUDPServerThread(); 
        int sockfd; 
        NamedThread *m_pUDPThread;
        std::string m_ControllerName;

        bool m_KeepAlive;

    };

}   // namespace CE::tcp

#endif
