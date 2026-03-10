#pragma once
#include "common.h"
#include <chrono>

class TcpHandler;

class ServerApp {
public:
    ServerApp();
    void run(TcpHandler &srv);
private:
    Config cfg;
    CFDConfig cfdConfig;
    ServerStatus status;
    int savedInput;
    bool mockTimerActive;
    std::chrono::steady_clock::time_point dataReceivedAt;
    void updateMockComputationStatus();
    void handleSendConfig(const std::string &msg);
    void handleGetStatus(TcpHandler &srv);
    void handleSendData(const std::string &msg, TcpHandler &srv);
    void handleGetResult(TcpHandler &srv);
};
