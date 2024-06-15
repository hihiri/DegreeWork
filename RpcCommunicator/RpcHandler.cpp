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

//TODO: verify the indexes and offsets work fine
void RpcHandler::UnWrap(string input) {
	Method = input.substr(input.find(declareRpcVersion, declareRpcVersion.length()), input.find(declareMethod));
	Params = input.substr(input.find(declareParams, declareParams.length()), input.find(declareId));
	Id = stoi(input.substr(input.find(declareId, declareId.length()), input.length() - 1 - input.find(declareId, declareId.length()) - 1));
}