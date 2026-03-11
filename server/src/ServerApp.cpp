#include "ServerApp.hpp"
#include "ConfigHandler.hpp"
#include "TcpHandler.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
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

void ServerApp::logMessage(const std::string &tag, const std::string &msg) const
{
    if(!cfg.log)
        return;

    std::ofstream f(LOG_PATH, std::ios::app);
    f << nowStr() << " [" << tag << "] ";
    for(unsigned char ch : msg){
        f << std::hex << std::setw(2) << std::setfill('0') << (int)ch << " ";
    }
    f << std::dec << "\n";
}

ServerApp::ServerApp()
    : cfg(ConfigHandler::read(CONFIG_PATH)),
      cfdConfig(),
      status(ServerStatus::Idle),
      savedInput(0),
      mockTimer() {}

void ServerApp::run(TcpHandler &srv)
{
    char buf[SOCKET_BUFFER_SIZE];
    while(true){
        int bytes = srv.recvData(buf, SOCKET_BUFFER_SIZE);
        if(bytes<=0) break;
        updateMockComputationStatus();
        std::string msg(buf, buf+bytes);
        logMessage("rx", msg);

        char mtype = msg[0];
        switch(mtype){
            case msgChar(MessageType::SendConfig):
                onConfigReceived(msg); break;
            case msgChar(MessageType::GetStatus):
                onStatusRequested(srv); break;
            case msgChar(MessageType::SendData):
                onDataReceived(msg, srv); break;
            case msgChar(MessageType::GetResult):
                onResultRequested(srv); break;
            default:
                // to be continued :)
            break;
        }
    }
}

void ServerApp::onConfigReceived(const std::string &msg)
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
            
            ConfigHandler::write(CONFIG_PATH, newc, cfdConfig);
            cfg = newc;
            
            std::cout << "Config overwritten: log=" << newc.log << "\n";
            std::cout << "CFD config: width=" << width << ", height=" << height << "\n";
            
            logMessage("info", "Received and saved config");
        }
    } catch(const std::exception &ex){
        std::cerr << "Failed to parse SendConfig: " << ex.what() << "\n";
        logMessage("error", std::string("Failed to parse SendConfig: ") + ex.what());
    }
}

void ServerApp::onStatusRequested(TcpHandler &srv)
{
    std::string resp;
    resp.push_back(msgChar(MessageType::StatusResponse));
    resp.push_back(char('0' + (statusCode(status) % 10)));
    srv.sendData(resp.c_str(), (int)resp.size());
    logMessage("tx", resp);
}

void ServerApp::onDataReceived(const std::string &msg, TcpHandler &srv)
{
    if(status != ServerStatus::Idle){
        std::string err(1, msgChar(MessageType::BusyError));
        srv.sendData(err.c_str(), (int)err.size());
        logMessage("tx", err);
    }
    else {
        bool receiveOk = false;

        try{
            status = ServerStatus::Computing;

            size_t payloadStartIndex = 0;
            int payloadSize = parsePayloadSize(msg, payloadStartIndex);
            std::vector<char> payload = receivePayload(msg, payloadStartIndex, payloadSize, srv);
            savePayload(payload);
            completeDataReceive(payload.size());
            receiveOk = true;
        } catch(const std::exception &ex) {
            rollbackDataReceive(ex.what());
        }

        if(receiveOk){
            std::string ack(1, msgChar(MessageType::DataAck));
            srv.sendData(ack.c_str(), (int)ack.size());
            logMessage("tx", ack);
        }
    }
}

int ServerApp::parsePayloadSize(const std::string &msg, size_t &payloadStartIndex) const
{
    size_t firstDelimiter = msg.find('|', 1);
    if(firstDelimiter == std::string::npos)
        throw std::runtime_error("Invalid SendData message: missing payload size delimiter");

    size_t secondDelimiter = msg.find('|', firstDelimiter + 1);
    if(secondDelimiter == std::string::npos)
        throw std::runtime_error("Invalid SendData message: missing payload start delimiter");

    int payloadSize = std::stoi(msg.substr(firstDelimiter + 1, secondDelimiter - firstDelimiter - 1));
    if(payloadSize < 0)
        throw std::runtime_error("Invalid SendData message: negative payload size");

    payloadStartIndex = secondDelimiter + 1;
    return payloadSize;
}

std::vector<char> ServerApp::receivePayload(const std::string &msg, size_t payloadStartIndex, int payloadSize, TcpHandler &srv) const
{
    std::vector<char> payload;
    payload.reserve((size_t)payloadSize);

    size_t available = msg.size() - payloadStartIndex;
    if(available > 0){
        size_t initialCopy = (available > (size_t)payloadSize) ? (size_t)payloadSize : available;
        payload.insert(payload.end(), msg.begin() + payloadStartIndex, msg.begin() + (payloadStartIndex + initialCopy));
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

    return payload;
}

void ServerApp::savePayload(const std::vector<char> &payload) const
{
    std::ofstream out(DATA_PATH, std::ios::binary | std::ios::trunc);
    out.write(payload.data(), (std::streamsize)payload.size());
    out.close();
}

void ServerApp::completeDataReceive(size_t payloadBytes)
{
    std::cout << "Received CFD payload bytes: " << payloadBytes << "\n";
    logMessage("info", std::string("Received CFD payload bytes: ") + std::to_string(payloadBytes));

    savedInput = 1;
    mockTimer.start();

    logMessage("info", "Mock computation started (5s timer)");
}

void ServerApp::rollbackDataReceive(const std::string &reason)
{
    status = ServerStatus::Idle;
    savedInput = 0;
    mockTimer.stop();
    std::cerr << "Failed to parse SendData: " << reason << "\n";
    logMessage("error", std::string("Failed to parse SendData: ") + reason);
}

void ServerApp::onResultRequested(TcpHandler &srv)
{
    switch(status){
        case ServerStatus::Idle: {
            std::string resp(1, msgChar(MessageType::ResultNotReadyError));
            resp += "noInput";
            srv.sendData(resp.c_str(), (int)resp.size());
            logMessage("tx", resp);
            break;
        }

        case ServerStatus::Computing: {
            std::string resp(1, msgChar(MessageType::ResultNotReadyError));
            resp += "computing";
            srv.sendData(resp.c_str(), (int)resp.size());
            logMessage("tx", resp);
            break;
        }

        case ServerStatus::Done:
        
        default: {
            std::string resp(1, msgChar(MessageType::ResultResponse));
            resp += "2026"; // Mock result data
            srv.sendData(resp.c_str(), (int)resp.size());
            logMessage("tx", resp);

            status = ServerStatus::Idle;
            savedInput = 0;
            mockTimer.stop();
            break;
        }
    }
}
