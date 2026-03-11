#pragma once
#include "common.h"
#include <chrono>

class TcpHandler;

class ServerApp {
public:
    ServerApp();
    void run(TcpHandler &srv);
private:
    struct MockComputationTimer {
        bool active;
        std::chrono::steady_clock::time_point startedAt;
        std::chrono::seconds duration;

        MockComputationTimer();
        void start();
        void stop();
        bool shouldCompleteNow() const;
    };

    Config cfg;
    CFDConfig cfdConfig;
    ServerStatus status;
    int savedInput;
    MockComputationTimer mockTimer;

    void updateMockComputationStatus();
    void handleSendConfig(const std::string &msg);
    void handleGetStatus(TcpHandler &srv);
    void handleSendData(const std::string &msg, TcpHandler &srv);
    void handleGetResult(TcpHandler &srv);
};
