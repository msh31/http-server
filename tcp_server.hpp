#pragma once
#include <string>
#include <iostream>
#include <ctime>
#include <fstream>
#include <sstream>
#include <thread>

#include "socket_wrapper.hpp"
#include "router.hpp"

#pragma once

namespace HTTP 
{
    class tcp_server
    {
        public:
            tcp_server(std::string ip_address, int port);
            ~tcp_server();   
            
            int startServer();
            void handleClient(SOCKET client_socket);
            std::string serveContent(const std::string& path);
            static std::string getContentType(const std::string& filepath);

        private:
            SOCKET m_socket;
            sockaddr_in m_server_address;
            std::string m_ip_address;
            int m_port;
            RequestRouter m_router;

            std::string getCurrentTime();
            void setupRoutes(); 
    };
}