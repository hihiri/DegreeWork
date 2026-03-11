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
    void onConfigReceived(const std::string &msg);
    void onStatusRequested(TcpHandler &srv);
    void onDataReceived(const std::string &msg, TcpHandler &srv);
    void onResultRequested(TcpHandler &srv);
};
