#include "ServerApp.hpp"

ServerApp::MockComputationTimer::MockComputationTimer()
    : active(false),
      startedAt(std::chrono::steady_clock::time_point{}),
      duration(std::chrono::seconds(5)) {}

void ServerApp::MockComputationTimer::start()
{
    active = true;
    startedAt = std::chrono::steady_clock::now();
}

void ServerApp::MockComputationTimer::stop()
{
    active = false;
    startedAt = std::chrono::steady_clock::time_point{};
}

bool ServerApp::MockComputationTimer::shouldCompleteNow() const
{
    if(!active)
        return false;

    auto now = std::chrono::steady_clock::now();
    return (now - startedAt) >= duration;
}
