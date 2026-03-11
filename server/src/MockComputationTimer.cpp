#include "ServerApp.hpp"
#include <iostream>
#include <thread>

void ServerApp::startMockComputation()
{
    std::thread([this](){
        std::this_thread::sleep_for(std::chrono::seconds(5));

        if(status.load() != ServerStatus::Computing)
            return;

        status.store(ServerStatus::Done);
        std::cout << "Mock computation finished -> status=Done\n";
    }).detach();
}
