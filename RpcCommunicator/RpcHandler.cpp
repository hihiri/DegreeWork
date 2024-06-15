#include "RpcHandler.hpp"
#include <string>

string RpcVersion = "2.0",
	declareRpcVersion = "{\"jsonrpc\":\"",
	declareMethod = "\",\"method\":",
	declareParams = ",\"params\":",
	declareId = ",\"id\":";

string RpcHandler::Wrap() {
	return declareRpcVersion + RpcVersion + declareMethod + Method + declareParams + Params + declareId + to_string(Id) + "}";
}

int findEndIndex(string input, string declaration) {
	return input.find(declaration, declaration.length());
}

//TODO: verify the indexes and offsets work fine
void RpcHandler::UnWrap(string input) {
	//extract method
	Method = input.substr(findEndIndex(input, declareRpcVersion)+1, input.find(declareMethod));
	//extract params
	int firstIndex = findEndIndex(input, declareParams) + 1;
	Params = input.substr(firstIndex, input.find(declareId) - firstIndex);
	//extract JsonId: lenght of id = input.length - 1 (for last '}') - first index
	int firstIndex = findEndIndex(input, declareId) + 1;
	Id = stoi(input.substr(firstIndex, input.length() - 1 - firstIndex));
}