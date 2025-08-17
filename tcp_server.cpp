#include "tcp_server.hpp"
#include <ctime>
#include <sstream>

namespace HTTP 
{    
    tcp_server::tcp_server(std::string ip_address, int port) {
        m_ip_address = ip_address;
        m_port = port;

        startServer();
    }

    tcp_server::~tcp_server() {
        if (m_socket != INVALID_SOCKET) {
            CLOSE_SOCKET(m_socket);
        }
    #ifdef _WIN32
        WSACleanup();
    #endif

        std::cout << "\ngoodbye!";
    }

    std::string tcp_server::getCurrentTime() {
        std::time_t now = std::time(nullptr);
        std::tm* local_time = std::localtime(&now);

        std::ostringstream oss;
        oss << (local_time->tm_year + 1900) << "-"
            << (local_time->tm_mon + 1) << "-"
            << local_time->tm_mday << " "
            << local_time->tm_hour << ":"
            << local_time->tm_min << ":"
            << local_time->tm_sec;

        return oss.str();
    }

    int tcp_server::startServer() {
    #ifdef _WIN32
        WSADATA wsaData;

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            return -1;
        }
    #endif

        m_server_address.sin_family = AF_INET;
        m_server_address.sin_port = htons(m_port);
        inet_pton(AF_INET, m_ip_address.c_str(), &m_server_address.sin_addr);
        m_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (m_socket == INVALID_SOCKET) {
            return -1;
        }
        
        if (bind(m_socket, SOCKADDR_CAST &m_server_address, sizeof(m_server_address)) == SOCKET_ERROR) {
            CLOSE_SOCKET(m_socket);
            return -1;
        }
        
        if (listen(m_socket, 5) == SOCKET_ERROR) {
            CLOSE_SOCKET(m_socket);
            return -1;
        }

        std::cout << "Server listening on " << m_ip_address << ":" << m_port << "\n";

        while(true) {
            sockaddr_in client_address;
            SOCKLEN_T client_address_size = sizeof(client_address);
            
            SOCKET client_socket = accept(m_socket, SOCKADDR_CAST &client_address, &client_address_size);
            
            if (client_socket == INVALID_SOCKET) {
                std::cout << "Accept failed" << "\n";
                continue;
            }
            
            std::cout << "Client connected!" << "\n";

            std::string html_content =
                "<html><head><title>Marco's HTTP Server</title></head><body>"
                "<h1>Welcome to my first HTTP server!</h1>"
                "<p>Built from scratch in C++</p>"
                "<p>Time: " + getCurrentTime() + "</p>"
                "</body></html>";

            std::string response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: " + std::to_string(html_content.length()) + "\r\n"
                "\r\n" +
                html_content;

            send(client_socket, response.c_str(), response.length(), 0);
            CLOSE_SOCKET(client_socket);
        }
    }
} 