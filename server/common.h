#pragma once
#include <string>

struct Config { int port=12345; bool log=false; };

struct CFDConfig { int width=4096; int height=32; };

enum class MessageType : char {
	SendConfig = '0',
	GetStatus = '1',
	SendData = '2',
	GetResult = '3',
	StatusResponse = '5',
	BusyError = '6',
	DataAck = '7',
	ResultNotReadyError = '8',
	ResultResponse = '9'
};

constexpr char msgChar(MessageType type) {
	return static_cast<char>(type);
}

#define CONFIG_PATH "server/config/config.json"
#define LOG_PATH "server/log/server.log"
#define DATA_PATH "server/data_payload.bin"
#define PORT "12345"
