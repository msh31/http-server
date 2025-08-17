#ifdef _WIN32
    #include <winsock2.h> //enables network programming 
    #include <ws2tcpip.h> //for inet_pton(), enebles modern IP address conversion

    #define CLOSE_SOCKET(s) closesocket(s)
    #define SOCKADDR_CAST (SOCKADDR *)
    #define SOCKET_ERROR (-1)
    #define SOCKLEN_T int 
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>

    #define CLOSE_SOCKET(s) close(s)
    #define SOCKET int
    #define INVALID_SOCKET (-1)
    #define SOCKADDR_CAST (struct sockaddr *)
    #define SOCKET_ERROR (-1)
    #define SOCKLEN_T socklen_t
#endif

class SocketWrapper {
private:
    SOCKET sock;

public:
    SocketWrapper(SOCKET s) : sock(s) {}
    
    ~SocketWrapper() { 
        if (sock != INVALID_SOCKET) CLOSE_SOCKET(sock); 
    }
    
    operator SOCKET() const { return sock; }
    
    SocketWrapper(const SocketWrapper&) = delete;
    SocketWrapper& operator=(const SocketWrapper&) = delete;
};