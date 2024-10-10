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
            string err = "Error at socket():" + WSAGetLastError();
            WSACleanup();
            throw err + "\n";
        }
        else {
            cout << "socket is OK!" << endl;
        }
    }

public:
    NetworkHandlerBase() {
        InitWSA();
        InitSocket();        
    }

    virtual char* WaitReceiving() = 0;
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

    char* WaitReceiving() override {
        char receiveBuffer[200];
        int rbyteCount = recv(AcceptSocket, receiveBuffer, 200, 0);
        if (rbyteCount < 0) {
            cout << "Server recv error: " << WSAGetLastError() << endl;
            return 0;
        }
        else {
            cout << "Received data: " << receiveBuffer << endl;
        }

        return receiveBuffer;
    }

    void Send(const char* message) override {
        char buffer[200];
        printf("Enter the message: ");
        cin.getline(buffer, 200);
        int sbyteCount = send(AcceptSocket, buffer, 200, 0);
        if (sbyteCount == SOCKET_ERROR) {
            throw "Server send error: " + WSAGetLastError();
        }
        else {
            cout << "Server: sent" << sbyteCount << endl;
        }
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
        int sbyteCount = send(mySocket, message, 200, 0);
        if (sbyteCount == SOCKET_ERROR) {
            throw "Server send error: " + WSAGetLastError();
        }
        else {
            cout << "Server: sent" << sbyteCount << endl;
        }
    }

    char* WaitReceiving() override {
        char receiveBuffer[200];
        int byteCount = recv(mySocket, receiveBuffer, 200, 0);
        if (byteCount < 0) {
            cout << "Server recv error: " << WSAGetLastError() << endl;
            return 0;
        }
        else {
            cout << "Received data: " << receiveBuffer << endl;
        }
    }
};
#endif