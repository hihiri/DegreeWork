#include <gtest/gtest.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <set>
#include <vector>

// Simulate send_msg and read_len logic for testing

uint32_t to_network_len(uint32_t len) {
    return htonl(len);
}

uint32_t from_network_len(const char* buf) {
    uint32_t len;
    memcpy(&len, buf, 4);
    return ntohl(len);
}

const std::set<std::string> allowed_msgs = {
    "GetStatus", "GetTaskQue", "QueTask", "RemoveTask", "EmptyQue", "PutConfig", "EndCurrentTask"
};

std::string handle_msg(const std::string& msg) {
    if (allowed_msgs.count(msg)) {
        if (msg == "GetStatus") return "Status: OK";
        if (msg == "GetTaskQue") return "TaskQue: []";
        if (msg == "QueTask") return "Task Queued";
        if (msg == "RemoveTask") return "Task Removed";
        if (msg == "EmptyQue") return "Queue Emptied";
        if (msg == "PutConfig") return "Config Updated";
        if (msg == "EndCurrentTask") return "Task Ended";
    }
    return "Error: Unknown message";
}

TEST(ServerTest, LengthEncodingDecoding) {
    uint32_t orig = 1234;
    uint32_t net = to_network_len(orig);
    char buf[4];
    memcpy(buf, &net, 4);
    uint32_t decoded = from_network_len(buf);
    ASSERT_EQ(orig, decoded);
}

TEST(ServerTest, MessageSerialization) {
    std::string msg = "hello";
    uint32_t len = htonl(msg.size());
    std::string packet(reinterpret_cast<char*>(&len), 4);
    packet += msg;
    ASSERT_EQ(packet.size(), msg.size() + 4);
    ASSERT_EQ(packet.substr(4), msg);
}

TEST(ServerTest, AllowedMessagesHandled) {
    for (const auto& msg : allowed_msgs) {
        std::string reply = handle_msg(msg);
        ASSERT_NE(reply, "Error: Unknown message");
        ASSERT_FALSE(reply.empty());
    }
}

TEST(ServerTest, DisallowedMessageHandled) {
    std::string reply = handle_msg("NotAllowed");
    ASSERT_EQ(reply, "Error: Unknown message");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
