#ifndef RPCHANDLER_HPP
#define RPCHANDLER_HPP

#include <iostream>

#include "MessageStructures.hpp"
#include <vector>
#include <string>

using namespace std;

string RpcVersion = "2.0",
declareRpcVersion = "jsonrpc",
declareMethod = "method",
declareParams = "params",
declareId = "id",
formatErr = "Bad format!";

class RpcHandler {
	int Id = 0;
	MethodType Method;
	vector<param*> Params;

	string Wrap(string serializedParams) {
		return "{\"" + declareRpcVersion + "\":\"" + RpcVersion + "\",\"" + declareMethod + "\":\"" + toString(Method) + "\",\"" + declareParams + "\":" + serializedParams + ",\"" + declareId + "\":" + to_string(Id) + "}";
	}

	string FindFirst(string key, vector<param*> v) {
		for (param* e : v)
			if (e->Key == key)
				return e->Value;
		throw "No suitable key found in vector!";
	}

	//apostroph is retained on string values after FindStructure
	string stripStringValue(string s) {
		return s.substr(1, s.length() - 2);
	}

	vector<param*> FindStructure(string input) {
		int depth = -1; //depth of parenthesis {{{}{}}}, -1 is special: it is a number 
		vector<param*> result = {};
		int state = 0;//tells wich index we are looking for
		int* indexes = new int[4]; // looking for 4 indexes: param name start / end, value start / end
		int iterationNumber = input.length();
		for (int i = 0; i < iterationNumber; i++) {
			switch (state) {
			case 0:
				if (input[i] == '"') {
					indexes[0] = i + 1;
					state = 1;
				}
				break;
			case 1:
				if (input[i] == '"') {
					indexes[1] = i;
					state++;
				}
				break;
			case 2:
				switch (input[i]) {
				case '{':
					indexes[2] = i;
					state++;
					depth = 1;
					break;
				case '"':
					indexes[2] = i + 1;
					state++;
					depth = 0;
					break;
				case ':':break;
				default:
					if (depth < 0 && isdigit(input[i]))
					{
						indexes[2] = i;
						state++;
					}
					else
						throw formatErr;
					break;
				}
				break;
			case 3:
				switch (depth) {
				case -1:
					if (input[i] == ',' || input[i] == '}' && i == input.length() - 1)
					{
						indexes[3] = i;
						state++;
					}
					break;
				case 0:
					if (input[i] == '"') {
						indexes[3] = i;
						state++;
					}
					break;
				default:
					switch (input[i]) {
					case '{': depth++; break;
					case '}': depth--; break;
					default: break;
					}
					if (depth == 0) {
						indexes[3] = i;
						state = 4;
					}
				}
				break;
			}
			if (state == 4)
			{
				string word1 = input.substr(indexes[0], indexes[1] - indexes[0]),
					word2 = input.substr(indexes[2], indexes[3] - indexes[2]);
				result.push_back(new param(word1, word2));
				state = 0;
				depth = -1;
			}
		}

		return result;
	}

	void UnWrap(string input) {
		vector<param*> envelope = FindStructure(input);
		Method = toMethodType(FindFirst(declareMethod, envelope));
		Id = atoi(FindFirst(declareId, envelope).c_str());
		Params = FindStructure(FindFirst(declareParams, envelope));
	}

	string SerializeParameters(vector<param*> attributes) {
		string output = "{";
		if (attributes.size() > 0) {
			for (param* e : attributes) {
				output += "\"" + e->Key + "\":" + e->Value;
			}
			output = output.substr(0, output.length()); //drop the extra ','
		}
		output += "}";

		return output;
	}

public:
	string Serialize(Message data) {
		Method = data.Method;
		return Wrap(SerializeParameters(data.toVector()));
	}

	Message Deserialize(string input) {
		UnWrap(input);
		switch (Method) {
			case loadParams: return *(new LoadParamsMessage(FindFirst("MessageContent", Params)));
			default: return *(new Message(Method));
		};			
	}
};

#endif