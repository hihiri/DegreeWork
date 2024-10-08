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

class MessageDataStructureBase {
public:
	MessageDataStructureBase(string method) {
		Method = method;
	};

	virtual vector<param*> toVector() { 
		return {};
	};

	string Method;
};

class TestMessage : public MessageDataStructureBase {
public:
	TestMessage(string method, string message) : MessageDataStructureBase(method){
		MessageContent = message;
		Method;
	}

	vector<param*> toVector() {
		return { new param("MessageContent", MessageContent)};
	}

	string MessageContent;
};

#endif