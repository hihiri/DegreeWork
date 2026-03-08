#pragma once
#include <string>

struct Config { int port=12345; bool log=false; };

struct CFDConfig { int width=4096; int height=32; };

#define CONFIG_PATH "server/config/config.json"
#define LOG_PATH "server/log/server.log"
#define PORT "12345"
