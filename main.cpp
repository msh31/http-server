#include "tcp_server.hpp"

using namespace HTTP;

int main() {    
    tcp_server server("127.0.0.1", 8004);
    
    if (server.startServer()) {
        std::cout << "Server started successfully on 127.0.0.1:8004" << "\n";
    } else {
        std::cout << "Failed to start server" << "\n";
        return 1;
    }
    
    return 0;
}