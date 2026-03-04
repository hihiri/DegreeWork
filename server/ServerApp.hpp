#pragma once
#include "common.h"

class TcpHandler;

class ServerApp {
public:
    ServerApp();
    void run(TcpHandler &srv);
private:
    Config cfg;
    int status;
    int savedInput;
    void handleSendConfig(const std::string &msg);
    void handleGetStatus(TcpHandler &srv);
    void handleSendData(const std::string &msg, TcpHandler &srv);
    void handleGetResult(TcpHandler &srv);
};
