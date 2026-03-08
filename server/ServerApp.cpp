#include "ServerApp.hpp"
#include "TcpHandler.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <cctype>
#include <ctime>

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
    write_field("height", std::to_string(cfd.height), true, 4);
    write_field("rho", std::to_string(cfd.rho), true, 4);
    write_field("rhoU", std::to_string(cfd.rhoU), true, 4);
    write_field("rhoV", std::to_string(cfd.rhoV), true, 4);
    write_field("energy", std::to_string(cfd.energy), false, 4);
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

ServerApp::ServerApp() : cfg(readConfig(CONFIG_PATH)), cfdConfig(), status(0), savedInput(0) {}

void ServerApp::run(TcpHandler &srv)
{
    const int BUF_SZ = 1024;
    char buf[BUF_SZ];
    while(true){
        int bytes = srv.recvData(buf, BUF_SZ);
        if(bytes<=0) break;
        std::string msg(buf, buf+bytes);
        if(cfg.log) logMessage(LOG_PATH, "rx", msg);

        unsigned char mtype = msg[0];
        if(mtype=='0'){
            handleSendConfig(msg);
        } else if(mtype=='1'){
            handleGetStatus(srv);
        } else if(mtype=='2'){
            handleSendData(msg, srv);
        } else if(mtype=='3'){
            handleGetResult(srv);
        } else {
            // to be continued :)
        }
    }
}

void ServerApp::handleSendConfig(const std::string &msg)
{
    // Parse: 0|log|width|height|rho|rhoU|rhoV|energy
    try{
        size_t pos = 1; // Skip '0'
        size_t delimiter = msg.find('|', pos);
        
        if(delimiter != std::string::npos) {
            int b = msg[pos] - '0';
            pos = delimiter + 1;
            
            delimiter = msg.find('|', pos);
            int width = std::stoi(msg.substr(pos, delimiter - pos));
            pos = delimiter + 1;
            
            delimiter = msg.find('|', pos);
            int height = std::stoi(msg.substr(pos, delimiter - pos));
            pos = delimiter + 1;
            
            delimiter = msg.find('|', pos);
            float rho = std::stof(msg.substr(pos, delimiter - pos));
            pos = delimiter + 1;
            
            delimiter = msg.find('|', pos);
            float rhoU = std::stof(msg.substr(pos, delimiter - pos));
            pos = delimiter + 1;
            
            delimiter = msg.find('|', pos);
            float rhoV = std::stof(msg.substr(pos, delimiter - pos));
            pos = delimiter + 1;
            
            float energy = std::stof(msg.substr(pos));
            
            Config newc;
            newc.port = cfg.port;
            newc.log = (b != 0);
            
            cfdConfig.width = width;
            cfdConfig.height = height;
            cfdConfig.rho = rho;
            cfdConfig.rhoU = rhoU;
            cfdConfig.rhoV = rhoV;
            cfdConfig.energy = energy;
            
            writeConfig(CONFIG_PATH, newc, cfdConfig);
            cfg = newc;
            
            std::cout << "Config overwritten: log=" << newc.log << "\n";
            std::cout << "CFD config: width=" << width << ", height=" << height
                      << ", rho=" << rho << ", rhoU=" << rhoU
                      << ", rhoV=" << rhoV << ", energy=" << energy << "\n";
            
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
    resp.push_back('5');
    resp.push_back(char('0' + (status % 10)));
    srv.sendData(resp.c_str(), (int)resp.size());
    if(cfg.log)
        logMessage(LOG_PATH, "tx", resp);
}

void ServerApp::handleSendData(const std::string &msg, TcpHandler &srv)
{
    if(status!=0){
        std::string err="6";
        srv.sendData(err.c_str(), (int)err.size());
        if(cfg.log)
            logMessage(LOG_PATH,"tx",err);
    }
    else {
        std::string payload = msg.substr(1);
        try{
            savedInput = std::stoi(payload);
        } catch(const std::exception &ex) {
            savedInput = 0;
            std::cerr << "Failed to parse SendData payload: " << ex.what() << "\n";
            if(cfg.log)
                logMessage(LOG_PATH, "error", std::string("Failed to parse SendData payload: ") + ex.what());
        }

        std::string ack = "7";
        srv.sendData(ack.c_str(), (int)ack.size());
        if(cfg.log)
            logMessage(LOG_PATH, "tx", ack);
        std::cout << "Saved input: " << savedInput << "\n";
    }
}

void ServerApp::handleGetResult(TcpHandler &srv)
{
    if(status!=1){
        std::string err="8";
        srv.sendData(err.c_str(), (int)err.size());
        if(cfg.log) logMessage(LOG_PATH,"tx",err);
    }
    else {
        std::string resp = "9" + std::string("2026");
        srv.sendData(resp.c_str(), (int)resp.size());
        if(cfg.log) logMessage(LOG_PATH, "tx", resp);
    }
}
