#pragma once
#include <winsock2.h>

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
    WSADATA wsaData{};
    SOCKET listenSocket = INVALID_SOCKET;
    SOCKET clientSocket = INVALID_SOCKET;
};
