#include <iostream>
#include <cstring>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <set>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include <queue>
#include "Task.h"
#include "server/MessageHandler.h"

using json = nlohmann::json;

class Server {
    Task task;

    Server() {
        taskQueue = std::queue<Task>();
    }

    std::string process_message(const std::string& msg) {
        try {
            MessageHandler m(msg);
            switch (m.getMethod()) {
                case MessageHandler::Method::GetStatus:
                    return task.status;
                case MessageHandler::Method::PutConfig:
                    return "not yet implemented";
                case MessageHandler::Method::PutTask:
                    task=Task(m.getPayload());
                    return "task started";
                case MessageHandler::Method::GetResult:
                    task=Task(m.getPayload());
                    return m.getResponse(task.status);
                default:
                    return "Error: Unknown message";
            }
        } catch (const std::exception& e) {
            return std::string("Error: ") + e.what();
        }
    }

    int main() {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(12345);
        bind(server_fd, (sockaddr*)&addr, sizeof(addr));
        listen(server_fd, 1);
        std::cout << "Server listening on port 12345\n";
        int client_fd = accept(server_fd, nullptr, nullptr);
        std::cout << "Client connected\n";
        std::string msg;
        while (read_msg(client_fd, msg)) {
            std::cout << "Received: " << msg << std::endl;
            std::string reply = process_message(msg);
            send_msg(client_fd, reply);
        }
        close(client_fd);
        close(server_fd);
        return 0;
    }
};
