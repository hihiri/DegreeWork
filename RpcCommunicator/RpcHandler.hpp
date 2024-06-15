#ifndef RPCHANDLER_HPP
#define RPCHANDLER_HPP

#include <iostream>
#include "MessageStructures.cpp"

using namespace std;

class RpcHandler {
	int Id = 0;
	string Method;
	string Params;

	string SerializeData(MessageDataStructureBase);
	MessageDataStructureBase DeserializeData(string);
	string Wrap();
	void UnWrap(string);

public:
	string Serialize(MessageDataStructureBase);
	MessageDataStructureBase Deserialize(string);
};

#endif