#include <iostream>

using namespace std;

enum CommandType {
	Test = 0
};

struct MessageDataStructureBase {
	MessageDataStructureBase(CommandType type) {
		Type = type;
	}

	CommandType Type;
};

struct TestMessage : MessageDataStructureBase {
	TestMessage(CommandType type, string message) : MessageDataStructureBase(type){
		MessageContent = message;
	}

	string MessageContent;
};