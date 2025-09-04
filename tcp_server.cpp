#include "tcp_server.hpp"

namespace HTTP 
{    
    tcp_server::tcp_server(std::string ip_address, int port) {
        m_ip_address = ip_address;
        m_port = port;

        setupRoutes();
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

    void tcp_server::setupRoutes() {
        // json api route
        m_router.addRoute("GET /api/time", [this](const HttpRequest&) -> std::string {
            std::string json = "{\"time\":\"" + getCurrentTime() + "\",\"status\":\"ok\"}";
            return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " +
                   std::to_string(json.length()) + "\r\n\r\n" + json;
        });

        m_router.addRoute("GET *", [this](const HttpRequest& req) -> std::string {
            return serveContent(req.path);
        });
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

            if (client_socket != INVALID_SOCKET) {
                std::thread client_thread(&tcp_server::handleClient, this, client_socket);
                client_thread.detach();
            }
        }
    }

    void tcp_server::handleClient(SOCKET client_socket) {
        char buffer[1024];
        int bytes = recv(client_socket, buffer, sizeof(buffer)-1, 0);

        std::cout << "Client connected!" << "\n";

        if (bytes > 0) {
            buffer[bytes] = '\0';

            HttpRequest request = m_router.parseHttpRequest(std::string(buffer));
            std::string response = m_router.handleRequest(request);

            send(client_socket, response.c_str(), response.length(), 0);
            std::cout << "=== Browser Request ===\n" << buffer << "\n";
        }

        CLOSE_SOCKET(client_socket);
    }

    std::string tcp_server::serveContent(const std::string& path) {
        std::string actualPath = path;

        if (path == "/") {
            actualPath = "/index.html";
        }
        std::string filepath = "./www" + actualPath;
        std::ifstream file(filepath, std::ios::binary);

        if (!file) {
            return "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found";
        }

        std::string content((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());

        std::string contentType = getContentType(filepath);

        return "HTTP/1.1 200 OK\r\nContent-Type: " + contentType +
               "\r\nContent-Length: " + std::to_string(content.length()) +
               "\r\n\r\n" + content;
    }

    std::string tcp_server::getContentType(const std::string& filepath) {
        if (filepath.rfind(".html") == filepath.length() - 5) return "text/html";
        if (filepath.rfind(".css") == filepath.length() - 4) return "text/css";
        if (filepath.rfind(".js") == filepath.length() - 3) return "application/javascript";
        if (filepath.rfind(".png") == filepath.length() - 4) return "image/png";
        if (filepath.rfind(".jpg") == filepath.length() - 4) return "image/jpeg";
        if (filepath.rfind(".jpeg") == filepath.length() - 5) return "image/jpeg";
        return "application/octet-stream";
    }
}
