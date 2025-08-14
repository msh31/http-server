#include <string>
#include <iostream>

#include "socket_wrapper.hpp"

#pragma once

namespace HTTP 
{
    class tcp_server
    {
        public:
            tcp_server(std::string ip_address, int port);
            ~tcp_server();   
            
            bool startServer();
            
        private:
            SOCKET m_socket;
            sockaddr_in m_server_address;
            std::string m_ip_address;
            int m_port;
    };
}