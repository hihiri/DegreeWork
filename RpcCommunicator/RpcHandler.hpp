#ifndef RPCHANDLER_HPP
#define RPCHANDLER_HPP

#include <iostream>
#include "MessageStructures.cpp"
#include <vector>

using namespace std;

class RpcHandler {
	int Id = 0;
	string Method;
	string Params;

	string Wrap();
	void UnWrap(string);
	void SerializeParameters(vector<Attribute*>);
	vector<Attribute*> DeserializeParameters();
public:
	string Serialize(MessageDataStructureBase);
	MessageDataStructureBase Deserialize(string);
};

#endif