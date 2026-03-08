#pragma once

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET SocketType;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    typedef int SocketType;
    #define INVALID_SOCKET -1
#endif

class TcpHandler {
public:
    explicit TcpHandler(const char* port);
    ~TcpHandler();

    bool listen();
    bool acceptClient();

    int recvData(char* buf, int bufsize);
    int sendData(const char* buf, int len);

    void closeClient();
    void closeListen();
    void cleanup();

private:
    const char* port;
#ifdef _WIN32
    WSADATA wsaData{};
#endif
    SocketType listenSocket = INVALID_SOCKET;
    SocketType clientSocket = INVALID_SOCKET;
};
