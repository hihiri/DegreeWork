#ifndef MESSAGESTRUCTURES_HPP
#define MESSAGESTRUCTURES_HPP

#include <iostream>
#include <vector>

using namespace std;

struct param {
	param(string key, string value) {
		Key = key;
		Value = value;
	}

	string Key;
	string Value;
};

enum MethodType {
	test,
	ENUM_SIZE
};

static const char* MethodTypeNames[] = { "test" };

static_assert(sizeof(MethodTypeNames) / sizeof(char*) == ENUM_SIZE, "type and type name array missmatch");

string toString(MethodType type) {
	return MethodTypeNames[type];
}

MethodType toMethodType(string method) {
	for (unsigned int i = 0; i < ENUM_SIZE; i++) {
		if (method == MethodTypeNames[i]) {
			return static_cast<MethodType>(i);
		}
	}
	throw "unkown method type: " + method + "\n";
}

class MessageDataStructureBase {
public:
	MessageDataStructureBase(MethodType method) {
		Method = method;
	};

	virtual vector<param*> toVector() { 
		return {};
	};

	MethodType Method;
};

class TestMessage : public MessageDataStructureBase {
public:
	TestMessage(string message) : MessageDataStructureBase(test){
		MessageContent = message;
	}

	vector<param*> toVector() {
		return { new param("MessageContent", MessageContent)};
	}

	string MessageContent;
};

#endif