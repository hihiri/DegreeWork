#include <iostream>
#include <vector>

using namespace std;

struct Attribute {
	Attribute(string key, string value) {
		Key = key;
		Value = value;
	}

	string Key;
	string Value;
};

class MessageDataStructureBase {
public:
	MessageDataStructureBase(string type) {
		Type = type;
	};

	virtual vector<Attribute*> toVector() { 
		return {};
	};

	string Type;
};

class TestMessage : public MessageDataStructureBase {
public:
	TestMessage(string type, string message) : MessageDataStructureBase(type){
		MessageContent = message;
	}

	vector<Attribute*> toVector() {
		return { new Attribute("Type", Type),new Attribute("MessageContent", MessageContent)};
	}

	string MessageContent;
};