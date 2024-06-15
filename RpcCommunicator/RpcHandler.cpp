#include "RpcHandler.hpp"
#include <string>
#include <stdexcept>
#include <vector>

string RpcVersion = "2.0",
	declareRpcVersion = "{\"jsonrpc\":\"",
	declareMethod = "\",\"method\":",
	declareParams = ",\"params\":",
	declareId = ",\"id\":";

//HELPER FUNCTIONS////////////////////////
size_t findEndIndex(string input, string declaration) {
	return input.find(declaration, declaration.length());
}

void dropFirst(string& s) {
	s = s.substr(1, s.length() - 1);
}

string popKey(string& s) {
	string output = s.substr(0, s.find("\""));
	s = s.substr(s.find("\""));

	return output;
}

string popValue(string& s) {
	size_t endIndex = s.find(",") < s.length() ? s.find(",") : s.find("}");
	string output = s.substr(0, endIndex);
	s = s.substr(endIndex);

	return output;
}

string RpcHandler::Wrap() {
	return declareRpcVersion + RpcVersion + declareMethod + Method + declareParams + Params + declareId + to_string(Id) + "}";
}

string FindFirst(string key, vector<Attribute*> v) {
	for (Attribute* e : v)
		if (e->Key == key)
			return e->Value;
	throw new exception("No suitable key found in vector!");
}
////////////////////////////////////////////

//TODO: verify the indexes and offsets work fine
void RpcHandler::UnWrap(string input) {
	//extract method
	Method = input.substr(findEndIndex(input, declareRpcVersion) + 1, input.find(declareMethod));
	//extract params
	size_t firstIndex = findEndIndex(input, declareParams) + 1;
	Params = input.substr(firstIndex, input.find(declareId) - firstIndex);
	//extract JsonId: lenght of id = input.length - 1 (for last '}') - first index
	firstIndex = findEndIndex(input, declareId) + 1;
	Id = stoi(input.substr(firstIndex, input.length() - 1 - firstIndex));
}

void RpcHandler::SerializeParameters(vector<Attribute*> attributes) {
	string output = "{";
	if (attributes.size() > 0) {
		for (Attribute* e : attributes) {
			output += "\"" + e->Key + "\":" + e->Value;
		}
		output = output.substr(0, output.length() - 1); //drop the extra ','
	}
	output += "}";

	Params = output;
}

//TODO: destructor for attributes!!!!
vector<Attribute*> RpcHandler::DeserializeParameters() {
	string input = Params;
	vector<Attribute*> attributes = {};
	dropFirst(input); //'{'
	while (input.length() > 1) {
		string key, value;

		dropFirst(input); //'"'
		key = popKey(input);
		dropFirst(input); //'"'
		dropFirst(input); //':'
		value = popValue(input);

		if (input != "}")
			dropFirst(input); //','

		attributes.push_back(new Attribute(key, value));
	}

	if (input != "}")
		throw new exception("Problem at param deserialization!");

	return attributes;
}

string RpcHandler::Serialize(MessageDataStructureBase data) {
	vector<Attribute*> attributes = {};
	if (data.Type == "test")
		attributes = dynamic_cast <TestMessage*>(&data)->toVector();
	SerializeParameters(attributes);
	return Wrap();
}

MessageDataStructureBase RpcHandler::Deserialize(string input) {
	UnWrap(input);
	vector<Attribute*> attributes = DeserializeParameters();

	if (Method == "test")
		return TestMessage("test", FindFirst("MessageContent", attributes));
	else
		throw new exception("Unexpected command type encountered");
}