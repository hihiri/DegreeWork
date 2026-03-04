#pragma once
#include <string>
#include "server/messageTypes.h"

class MessageHandler {
public:
    enum class Method {
        Unknown,
        GetStatus,
        PutConfig,
        PutTask,
        GetResult
    };

private:
    std::string payload;
    Method method;

public:
    MessageHandler(const std::string& input) {
        size_t delim = input.find(':');
        if (delim == std::string::npos) {
            method = Method::Unknown;
            payload.clear();
            return;
        }
        std::string methodStr = input.substr(0, delim);
        payload = input.substr(delim + 1);

        if (methodStr == "GetStatus") method = Method::GetStatus;
        else if (methodStr == "PutConfig") method = Method::PutConfig;
        else if (methodStr == "PutTask") method = Method::PutTask;
        else if (methodStr == "GetResult") method = Method::GetResult;
        else method = Method::Unknown;
    }

    Method getMethod() const {
        return method;
    }

    const std::string& getPayload() const {
        return payload;
    }
};