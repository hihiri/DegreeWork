#ifndef NETWORKHANDLER_HPP
#define NETWORKHANDLER_HPP

#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

class NetworkHandlerBase {
protected:
    SOCKET mySocket;

    void InitWSA() {
        WSADATA wsaData;
        int wsaerr;
        WORD wVersionRequested = MAKEWORD(2, 2);
        wsaerr = WSAStartup(wVersionRequested, &wsaData);
        if (wsaerr != 0) {
            throw "The Winsock dll not found!";
        }
        else {
            cout << "The Winsock dll found" << endl;
            cout << "The status: " << wsaData.szSystemStatus << endl;
        }
    }

    SOCKET InitSocket() {
        mySocket = INVALID_SOCKET;
        mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (mySocket == INVALID_SOCKET) {
            string err = string("Error at socket():") + to_string(WSAGetLastError());
            WSACleanup();
            throw err + "\n";
        }
        else {
            cout << "socket is OK!" << endl;
        }
        return mySocket;
    }

    string BaseWaitReceive(SOCKET _socket) {
        char receiveBuffer[201];
        int byteCount = recv(_socket, receiveBuffer, 200, 0);
        if (byteCount < 0) {
            throw string("Server recv error: ") + to_string(WSAGetLastError());
        }
        else {
            // ensure null termination
            int len = byteCount < 200 ? byteCount : 200;
            receiveBuffer[len] = '\0';
            cout << "Received data: " << receiveBuffer << endl;
            return string(receiveBuffer);
        }
    }

    void BaseSend(const char* message, SOCKET _socket) {
        size_t toSend = strlen(message);
        int sbyteCount = send(_socket, message, static_cast<int>(toSend), 0);
        if (sbyteCount == SOCKET_ERROR) {
            throw string("Server send error: ") + to_string(WSAGetLastError());
        }
        else {
            cout << "Server: sent " << sbyteCount << endl;
        }
    }
public:
    NetworkHandlerBase() {
        InitWSA();
        InitSocket();        
    }

    virtual string WaitReceive() = 0;
    virtual void Send(const char*) = 0;
};

class ServerNetworkHandler : public NetworkHandlerBase {
    SOCKET AcceptSocket;

    void BindSocket() {
        sockaddr_in service;
        service.sin_family = AF_INET;
        inet_pton(AF_INET, "127.0.0.1", &service.sin_addr.s_addr);
        service.sin_port = htons(55555);
        if (bind(mySocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
            string err = "bind() failed: " + WSAGetLastError();;
            closesocket(mySocket);
            WSACleanup();
            throw err;
        }
        else {
            cout << "bind() is OK!" << endl;
        }
    }

    void StartListenForConnection() {
        if (listen(mySocket, 1) == SOCKET_ERROR) {
            cout << "listen(): Error listening on socket: " << WSAGetLastError() << endl;
        }
        else {
            cout << "listen() is OK!, I'm waiting for new connections..." << endl;
        }
    }

    void Accept() {
        AcceptSocket = accept(mySocket, NULL, NULL);
        if (AcceptSocket == INVALID_SOCKET) {
            string err = "accept failed: " + WSAGetLastError();
            WSACleanup();
            throw err;
        }
        else {
            cout << "accept() is OK!" << endl;
        }
    }

public:
    ServerNetworkHandler() {
        BindSocket();
        StartListenForConnection();
        Accept();
    }

    string WaitReceive() override {
        return BaseWaitReceive(AcceptSocket);
    }

    void Send(const char* message) override {
        BaseSend(message, AcceptSocket);
    }
};

class ClientNetworkHandler : public NetworkHandlerBase {
public:
    ClientNetworkHandler(){
        sockaddr_in clientService;
        clientService.sin_family = AF_INET;
        inet_pton(AF_INET, "127.0.0.1", &clientService.sin_addr.s_addr);
        clientService.sin_port = htons(55555);
        if (connect(mySocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
            string err = "Client: connect() - Failed to connect: " + WSAGetLastError();
            WSACleanup();
            throw err;
        }
        else {
            cout << "Client: Connect() is OK!" << endl;
            cout << "Client: Can start sending and receiving data..." << endl;
        }
    }

    void Send(const char* message) override {
        BaseSend(message, mySocket);
    }

    string WaitReceive() override {
        return BaseWaitReceive(mySocket);
    }
};
#endif