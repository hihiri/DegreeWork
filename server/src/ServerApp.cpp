#include "ServerApp.hpp"
#include "TcpHandler.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <cctype>
#include <ctime>
#include <vector>
#include <stdexcept>
#include <chrono>

static std::string nowStr(){
    std::time_t t = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return std::string(buf);
}

static int parseIntAfter(const std::string &s, size_t pos){
    size_t colon = s.find(':', pos);
    if(colon==std::string::npos)
        return 0;
    size_t i = colon+1;
    while(i<s.size() && (s[i]==' '||s[i]=='"'))
        ++i;
    std::string num;
    while(i<s.size() && (s[i]=='-' || std::isdigit((unsigned char)s[i]))) {
        num.push_back(s[i]);
        ++i;
    }
    if(num.empty())
        return 0;
    return std::stoi(num);
}

static Config readConfig(const std::string &path){
    Config c;
    std::ifstream f(path);
    if(!f)
        return c;
    std::string s((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    auto parseIntField = [&](const std::string &key, int &out){
        size_t pos = s.find(key);
        if(pos!=std::string::npos)
            out = parseIntAfter(s,pos);
    };

    auto parseBoolField = [&](const std::string &key, bool &out){
        size_t pos = s.find(key);
        if(pos!=std::string::npos)
           out = (parseIntAfter(s,pos)!=0);
    };

    parseIntField("\"port\"", c.port);
    parseBoolField("\"log\"", c.log);
    return c;
}

static void writeConfig(const std::string &path, const Config &c, const CFDConfig &cfd){
    std::ofstream f(path, std::ios::trunc);
    f << "{\n";
    auto write_field = [&](const char *name, const std::string &value, bool comma, int indent = 2){
        f << std::string(indent, ' ') << "\"" << name << "\": " << value;
        if(comma) f << ",";
        f << "\n";
    };

    write_field("log", (c.log?"true":"false"), true);
    write_field("port", std::to_string(c.port), true);
    
    f << "  \"cfdConfig\": {\n";
    write_field("width", std::to_string(cfd.width), true, 4);
    write_field("height", std::to_string(cfd.height), false, 4);
    f << "  }\n";
    
    f << "}\n";
}

static void logMessage(const std::string &logpath, const std::string &tag, const std::string &msg){
    std::ofstream f(logpath, std::ios::app);
    f << nowStr() << " [" << tag << "] ";
    for(unsigned char ch : msg){
        f << std::hex << std::setw(2) << std::setfill('0') << (int)ch << " ";
    }
    f << std::dec << "\n";
}

ServerApp::ServerApp()
    : cfg(readConfig(CONFIG_PATH)),
      cfdConfig(),
      status(ServerStatus::Idle),
      savedInput(0),
      mockTimer() {}

void ServerApp::updateMockComputationStatus()
{
    if(status != ServerStatus::Computing)
        return;

    if(mockTimer.shouldCompleteNow()){
        status = ServerStatus::Done;
        mockTimer.stop();
        std::cout << "Mock computation finished -> status=Done\n";
        if(cfg.log)
            logMessage(LOG_PATH, "info", "Mock computation finished -> status=Done");
    }
}

void ServerApp::run(TcpHandler &srv)
{
    char buf[SOCKET_BUFFER_SIZE];
    while(true){
        int bytes = srv.recvData(buf, SOCKET_BUFFER_SIZE);
        if(bytes<=0) break;
        updateMockComputationStatus();
        std::string msg(buf, buf+bytes);
        if(cfg.log) logMessage(LOG_PATH, "rx", msg);

        char mtype = msg[0];
        if(mtype==msgChar(MessageType::SendConfig)){
            handleSendConfig(msg);
        } else if(mtype==msgChar(MessageType::GetStatus)){
            handleGetStatus(srv);
        } else if(mtype==msgChar(MessageType::SendData)){
            handleSendData(msg, srv);
        } else if(mtype==msgChar(MessageType::GetResult)){
            handleGetResult(srv);
        } else {
            // to be continued :)
        }
    }
}

void ServerApp::handleSendConfig(const std::string &msg)
{
    // Parse: 0|log|width|height
    try{
        size_t pos = 1; // Skip '0'
        size_t delimiter = msg.find('|', pos);
        
        if(delimiter != std::string::npos) {
            int b = msg[pos] - '0';
            pos = delimiter + 1;
            
            delimiter = msg.find('|', pos);
            int width = std::stoi(msg.substr(pos, delimiter - pos));
            pos = delimiter + 1;
            
            int height = std::stoi(msg.substr(pos));
            
            Config newc;
            newc.port = cfg.port;
            newc.log = (b != 0);
            
            cfdConfig.width = width;
            cfdConfig.height = height;
            
            writeConfig(CONFIG_PATH, newc, cfdConfig);
            cfg = newc;
            
            std::cout << "Config overwritten: log=" << newc.log << "\n";
            std::cout << "CFD config: width=" << width << ", height=" << height << "\n";
            
            if(cfg.log)
                logMessage(LOG_PATH, "info", "Received and saved config");
        }
    } catch(const std::exception &ex){
        std::cerr << "Failed to parse SendConfig: " << ex.what() << "\n";
        if(cfg.log)
            logMessage(LOG_PATH, "error", std::string("Failed to parse SendConfig: ") + ex.what());
    }
}

void ServerApp::handleGetStatus(TcpHandler &srv)
{
    std::string resp;
    resp.push_back(msgChar(MessageType::StatusResponse));
    resp.push_back(char('0' + (statusCode(status) % 10)));
    srv.sendData(resp.c_str(), (int)resp.size());
    if(cfg.log)
        logMessage(LOG_PATH, "tx", resp);
}

void ServerApp::handleSendData(const std::string &msg, TcpHandler &srv)
{
    if(status != ServerStatus::Idle){
        std::string err(1, msgChar(MessageType::BusyError));
        srv.sendData(err.c_str(), (int)err.size());
        if(cfg.log)
            logMessage(LOG_PATH,"tx",err);
    }
    else {
        bool receiveOk = false;

        // Parse: 2|payloadSize|<binary payload>
        try{
            size_t firstDelimiter = msg.find('|', 1);
            if(firstDelimiter == std::string::npos)
                throw std::runtime_error("Invalid SendData message: missing payload size delimiter");

            size_t secondDelimiter = msg.find('|', firstDelimiter + 1);
            if(secondDelimiter == std::string::npos)
                throw std::runtime_error("Invalid SendData message: missing payload start delimiter");

            int payloadSize = std::stoi(msg.substr(firstDelimiter + 1, secondDelimiter - firstDelimiter - 1));
            if(payloadSize < 0)
                throw std::runtime_error("Invalid SendData message: negative payload size");

            status = ServerStatus::Computing;

            std::vector<char> payload;
            payload.reserve((size_t)payloadSize);

            size_t available = msg.size() - (secondDelimiter + 1);
            if(available > 0){
                size_t initialCopy = (available > (size_t)payloadSize) ? (size_t)payloadSize : available;
                payload.insert(payload.end(), msg.begin() + (secondDelimiter + 1), msg.begin() + (secondDelimiter + 1 + initialCopy));
            }

            char buf[SOCKET_BUFFER_SIZE];
            while((int)payload.size() < payloadSize){
                int bytes = srv.recvData(buf, SOCKET_BUFFER_SIZE);
                if(bytes <= 0)
                    throw std::runtime_error("Connection closed before full payload was received");

                size_t missing = (size_t)(payloadSize - (int)payload.size());
                size_t chunk = ((size_t)bytes > missing) ? missing : (size_t)bytes;
                payload.insert(payload.end(), buf, buf + chunk);
            }

            std::ofstream out(DATA_PATH, std::ios::binary | std::ios::trunc);
            out.write(payload.data(), (std::streamsize)payload.size());
            out.close();

            std::cout << "Received CFD payload bytes: " << payload.size() << "\n";
            if(cfg.log)
                logMessage(LOG_PATH, "info", std::string("Received CFD payload bytes: ") + std::to_string(payload.size()));

            savedInput = 1;
            mockTimer.start();
            receiveOk = true;

            if(cfg.log)
                logMessage(LOG_PATH, "info", "Mock computation started (5s timer)");
        } catch(const std::exception &ex) {
            status = ServerStatus::Idle;
            savedInput = 0;
            mockTimer.stop();
            std::cerr << "Failed to parse SendData: " << ex.what() << "\n";
            if(cfg.log)
                logMessage(LOG_PATH, "error", std::string("Failed to parse SendData: ") + ex.what());
        }

        if(receiveOk){
            std::string ack(1, msgChar(MessageType::DataAck));
            srv.sendData(ack.c_str(), (int)ack.size());
            if(cfg.log)
                logMessage(LOG_PATH, "tx", ack);
        }
    }
}

void ServerApp::handleGetResult(TcpHandler &srv)
{
    if(status == ServerStatus::Idle){
        std::string resp(1, msgChar(MessageType::ResultNotReadyError));
        resp += "noInput";
        srv.sendData(resp.c_str(), (int)resp.size());
        if(cfg.log) logMessage(LOG_PATH,"tx",resp);
    }
    else if(status == ServerStatus::Computing){
        std::string resp(1, msgChar(MessageType::ResultNotReadyError));
        resp += "computing";
        srv.sendData(resp.c_str(), (int)resp.size());
        if(cfg.log) logMessage(LOG_PATH,"tx",resp);
    }
    else {
        std::string resp(1, msgChar(MessageType::ResultResponse));
        resp += "2026"; // Mock result data
        srv.sendData(resp.c_str(), (int)resp.size());
        if(cfg.log) logMessage(LOG_PATH, "tx", resp);

        status = ServerStatus::Idle;
        savedInput = 0;
        mockTimer.stop();
    }
}
