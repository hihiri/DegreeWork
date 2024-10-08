#ifndef RPCHANDLER_HPP
#define RPCHANDLER_HPP

#include <iostream>

#include "MessageStructures.hpp"
#include <vector>

using namespace std;

class RpcHandler {
	int Id = 0;
	string Method;
	vector<param*> Params;

	string Wrap(string);
	void UnWrap(string);
	string SerializeParameters(vector<param*>);
public:
	string Serialize(TestMessage);
	TestMessage Deserialize(string);
};

#endif