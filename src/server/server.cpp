#include <iostream>
#include <cstring>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <set>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp> // You need to have nlohmann/json installed

using json = nlohmann::json;

std::set<std::string> load_allowed_msgs(const std::string& config_path = "c:/Users/oem user/communicator/config.json") {
    std::set<std::string> allowed = {"GetStatus", "PutConfig"};
    std::ifstream f(config_path);
    if (f) {
        json cfg;
        f >> cfg;
        if (cfg.contains("allowed_message_types")) {
            for (const auto& msg : cfg["allowed_message_types"]) {
                allowed.insert(msg.get<std::string>());
            }
        }
    }
    return allowed;
}

//placeholder yet
std::string handle_msg(const std::string& msg, const std::set<std::string>& allowed_msgs) {
    if (allowed_msgs.count(msg)) {
        if (msg == "GetStatus") return "Status: OK";
        if (msg == "PutConfig") return "Config Updated";
        if (msg == "GetTaskQue") return "TaskQue: []";
        if (msg == "QueTask") return "Task Queued";
        if (msg == "RemoveTask") return "Task Removed";
        if (msg == "EmptyQue") return "Queue Emptied";
        if (msg == "EndCurrentTask") return "Task Ended";
    }
    return "Error: Unknown message";
}

int main() {
    std::set<std::string> allowed_msgs = load_allowed_msgs();
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
        std::string reply = handle_msg(msg, allowed_msgs);
        send_msg(client_fd, reply);
    }
    close(client_fd);
    close(server_fd);
    return 0;
}
