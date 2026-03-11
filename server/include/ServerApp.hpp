#pragma once
#include "common.h"
#include <atomic>
#include <vector>

class TcpHandler;

class ServerApp {
public:
    ServerApp();
    void run(TcpHandler &srv);
private:
    Config cfg;
    CFDConfig cfdConfig;
    std::atomic<ServerStatus> status;
    int savedInput;

    void startMockComputation(); //for testing
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
