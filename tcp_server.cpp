#include "tcp_server.hpp"

namespace HTTP 
{    
    tcp_server::tcp_server(std::string ip_address, int port) {
        m_ip_address = ip_address;
        m_port = port;
    }

    tcp_server::~tcp_server() {
        if (m_socket != INVALID_SOCKET) {
            CLOSE_SOCKET(m_socket);
        }
    #ifdef _WIN32
        WSACleanup();
    #endif
    }

    bool tcp_server::startServer() {
    #ifdef _WIN32
        WSADATA wsaData;

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            return false;
        }
    #endif

        m_server_address.sin_family = AF_INET;
        m_server_address.sin_port = htons(m_port);
        inet_pton(AF_INET, m_ip_address.c_str(), &m_server_address.sin_addr);
        m_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (m_socket == INVALID_SOCKET) {
            return false;
        }
        
        if (bind(m_socket, SOCKADDR_CAST &m_server_address, sizeof(m_server_address)) == SOCKET_ERROR) {
            CLOSE_SOCKET(m_socket);
            return false;
        }
        
        if (listen(m_socket, 5) == SOCKET_ERROR) {
            CLOSE_SOCKET(m_socket);
            return false;
        }

        return true;
    }
} 