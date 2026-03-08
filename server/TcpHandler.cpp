#include "TcpHandler.hpp"
#include <iostream>

TcpHandler::TcpHandler(const char* port): port(port) {
#ifdef _WIN32
    WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
}

TcpHandler::~TcpHandler(){ cleanup(); }

bool TcpHandler::listen()
{
    addrinfo hints{}; addrinfo *result = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if(getaddrinfo(nullptr, port, &hints, &result)!=0){
        std::cerr<<"getaddrinfo failed\n";
        return false;
    }

    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(listenSocket==INVALID_SOCKET){
        std::cerr<<"socket failed\n";
        freeaddrinfo(result);
        return false;
    }

#ifdef _WIN32
    if(bind(listenSocket, result->ai_addr, (int)result->ai_addrlen)==SOCKET_ERROR){
#else
    if(bind(listenSocket, result->ai_addr, (int)result->ai_addrlen)==-1){
#endif
        std::cerr<<"bind failed\n";
        freeaddrinfo(result);
#ifdef _WIN32
        closesocket(listenSocket);
#else
        close(listenSocket);
#endif
        listenSocket=INVALID_SOCKET;
        return false;
    }
    freeaddrinfo(result);

#ifdef _WIN32
    if(::listen(listenSocket, SOMAXCONN)==SOCKET_ERROR){
#else
    if(::listen(listenSocket, SOMAXCONN)==-1){
#endif
        std::cerr<<"listen failed\n";
#ifdef _WIN32
        closesocket(listenSocket);
#else
        close(listenSocket);
#endif
        listenSocket=INVALID_SOCKET;
        return false;
    }
    return true;
}

bool TcpHandler::acceptClient()
{
    clientSocket = accept(listenSocket, nullptr, nullptr);
    if(clientSocket==INVALID_SOCKET){
        std::cerr<<"accept failed\n";
        return false;
    }
    return true;
}

int TcpHandler::recvData(char* buf, int bufsize)
{
    if(clientSocket==INVALID_SOCKET)
        return -1;
    return ::recv(clientSocket, buf, bufsize, 0);
}

int TcpHandler::sendData(const char* buf, int len)
{
    if(clientSocket==INVALID_SOCKET)
        return -1;
    return ::send(clientSocket, buf, len, 0);
}

void TcpHandler::closeClient()
{
    if(clientSocket!=INVALID_SOCKET){
#ifdef _WIN32
        closesocket(clientSocket);
#else
        close(clientSocket);
#endif
        clientSocket = INVALID_SOCKET;
    }
}

void TcpHandler::closeListen()
{
    if(listenSocket!=INVALID_SOCKET){
#ifdef _WIN32
        closesocket(listenSocket);
#else
        close(listenSocket);
#endif
        listenSocket = INVALID_SOCKET;
    }
}

void TcpHandler::cleanup()
{
    closeClient();
    closeListen();
#ifdef _WIN32
    WSACleanup();
#endif
}
