#ifndef MESSAGESTRUCTURES_HPP
#define MESSAGESTRUCTURES_HPP

#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct param {
    param(const string& key, const string& value) {
        Key = key;
        Value = value;
    }

    string Key;
    string Value;
};

enum MethodType {
	response,
	loadParams,
	startCalculation,
	getStatus,
	ENUM_SIZE
};

static const char* MethodTypeNames[] = { "Response", "LoadParameters", "StartCalculation", "GetStatus" };

static_assert(sizeof(MethodTypeNames) / sizeof(char*) == ENUM_SIZE, "type and type name array missmatch");

inline string toString(MethodType type) {
    return MethodTypeNames[type];
}

inline MethodType toMethodType(const string& method) {
    for (unsigned int i = 0; i < ENUM_SIZE; i++) {
        if (method == MethodTypeNames[i]) {
            return static_cast<MethodType>(i);
        }
    }
    throw string("unkown method type: " + method + "\n");
}

class Message {
public:
    explicit Message(MethodType method) : Method(method) {}
    virtual ~Message() = default;
    virtual vector<param> toVector() {
        return {};
    }

    MethodType Method;
    string ResponseMessage;
};

class LoadParamsMessage : public Message {
public:
    explicit LoadParamsMessage(const string& message) : Message(loadParams), MessageContent(message) {}

    vector<param> toVector() override {
        return { param("MessageContent", MessageContent) };
    }

    string MessageContent;
};

class Response : public Message {
public:
    explicit Response(const string& message) : Message(response) {
        Method = response;
        ResponseMessage = message;
    }
};

#endif
