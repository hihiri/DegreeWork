#pragma once
#include "common.h"
#include <chrono>
#include <vector>

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

    // Helpers: logging
    void logMessage(const std::string &tag, const std::string &msg) const;

    // Helpers: payload handling
    int parsePayloadSize(const std::string &msg, size_t &payloadStartIndex) const;
    std::vector<char> receivePayload(const std::string &msg, size_t payloadStartIndex, int payloadSize, TcpHandler &srv) const;
    void savePayload(const std::vector<char> &payload) const;

    // Helpers: state transitions
    void completeDataReceive(size_t payloadBytes);
    void rollbackDataReceive(const std::string &reason);
};
