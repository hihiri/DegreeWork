#include <iostream>
#include "MessageStructures.cpp"

using namespace std;

class RpcHandler {
	string Wrap(string input);
	string UnWrap(string input);
public:
	string SerializeMessage(MessageDataStructureBase message);
	MessageDataStructureBase DeserializeMessage(string message);
};