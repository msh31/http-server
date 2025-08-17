#include "tcp_server.hpp"

using namespace HTTP;

int main() {    
    tcp_server server("127.0.0.1", 8004);
    return 0;
}