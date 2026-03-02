#pragma once
#include <string>
#include <chrono>

class Task {
    int Id;
    int status=0; // 0 = pending, 1 = in-progress, 2 = completed 

    static int nextId;
    
    std::chrono::system_clock::time_point receiveTime;
    std::chrono::system_clock::time_point startTimeTime;
    std::string data;
public:

    Task(const std::string& data)
        : Id(nextId++), receiveTime(std::chrono::system_clock::now()), data(data) {}
};

int Task::nextId = 1;
