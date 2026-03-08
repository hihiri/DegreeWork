#pragma once
#include <string>

struct Config { int port=12345; bool log=false; };

struct CFDConfig { int width=4096; int height=32; float rho=1.0f; float rhoU=3.0f; float rhoV=0.0f; float energy=6.2857f; };

#define CONFIG_PATH "server/config/config.json"
#define LOG_PATH "server/log/server.log"
#define PORT "12345"
