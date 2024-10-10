#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "RpcHandler.hpp"

using namespace std;

int main() {
    //WSAStartup
    WSADATA wsaData;
    int wserr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wserr = WSAStartup(wVersionRequested, &wsaData);
    if (wserr != 0) {
        cout << "The winsock dll not found" << endl;
        return 0;
    }
    else {
        cout << "The Winsock dll found" << endl;
        cout << "The status: " << wsaData.szSystemStatus << endl;
    }

    //socket creation
    SOCKET clientSocket;
    clientSocket = INVALID_SOCKET;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 0;
    }
    else {
        cout << "socket is OK!" << endl;
    }

    //connection to server
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &clientService.sin_addr.s_addr);
    clientService.sin_port = htons(55555);
    if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        cout << "Client: connect() - Failed to connect: " << WSAGetLastError() << endl;
        WSACleanup();
        return 0;
    }
    else {
        cout << "Client: Connect() is OK!" << endl;
        cout << "Client: Can start sending and receiving data..." << endl;
    }

    //sending data
    printf("Enter the message: ");

    auto rpcHandler = new RpcHandler();

    int sbyteCount = send(clientSocket, rpcHandler->Serialize(*new TestMessage("6789123456")).c_str(), 200, 0);
    if (sbyteCount == SOCKET_ERROR) {
        cout << "Server send error: " << WSAGetLastError() << endl;
        return -1;
    }
    else {
        cout << "Server: sent" << sbyteCount << endl;
    }

    //receiving messages
     char receiveBuffer[200];
     int byteCount = recv(clientSocket, receiveBuffer, 200, 0);
     if(byteCount < 0){
         cout << "Server recv error: " << WSAGetLastError() << endl;
         return 0;
     } else {
         cout << "Received data: " << receiveBuffer << endl;
     }
}