#ifndef RPCHANDLER_HPP
#define RPCHANDLER_HPP

#include <cstdlib>
#include <cctype>
#include <vector>
#include <string>
#include "MessageStructures.hpp"

using namespace std;

static const string RpcVersion = "2.0";
static const string declareRpcVersion = "jsonrpc";
static const string declareMethod = "method";
static const string declareParams = "params";
static const string declareResponse = "result";
static const string declareId = "id";
static const string formatErr = "Bad format!";
static const string noKey = "No suitable key found in vector!";

class RpcHandler {
    int Id = 0;
    MethodType Method;
    vector<param> Params;

    string Wrap(const string& serializedParams) {
        string methodDeclaration = Method != response ? (declareMethod + "\":\"" + toString(Method) + "\",\"") : "";
        string resultOrParamDeclaration = Method != response ? declareParams : declareResponse;

        return "{\"" + declareRpcVersion + "\":\"" + RpcVersion + "\",\"" + methodDeclaration + resultOrParamDeclaration + ":" + serializedParams + ",\"" + declareId + "\":" + to_string(Id) + "}";
    }

    string FindFirst(const string& key, const vector<param>& v) const {
        for (const param& e : v)
            if (e.Key == key)
                return e.Value;
        throw string(noKey);
    }

    // apostroph is retained on string values after FindStructure
    string stripStringValue(const string& s) {
        return s.substr(1, s.length() - 2);
    }

    vector<param> FindStructure(const string& input) {
        int depth = -1; // depth of parenthesis {{{}{}}}, -1 is special: it is a number 
        vector<param> result;
        int state = 0; // tells which index we are looking for
        int indexes[4] = {0,0,0,0}; // param name start/end, value start/end
        int iterationNumber = static_cast<int>(input.length());
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
                case ':': break;
                default:
                    if (depth < 0 && isdigit(static_cast<unsigned char>(input[i])))
                    {
                        indexes[2] = i;
                        state++;
                    }
                    else
                        throw string(formatErr);
                    break;
                }
                break;
            case 3:
                switch (depth) {
                case -1:
                    if (input[i] == ',' || (input[i] == '}' && i == input.length() - 1))
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
                string word1 = input.substr(indexes[0], indexes[1] - indexes[0]);
                string word2 = input.substr(indexes[2], indexes[3] - indexes[2]);
                result.emplace_back(word1, word2);
                state = 0;
                depth = -1;
            }
        }

        return result;
    }

    void UnWrap(const string& input) {
        vector<param> envelope = FindStructure(input);
        try {
            Method = toMethodType(FindFirst(declareMethod, envelope));
            // parse inner params
            Params = FindStructure(FindFirst(declareParams, envelope));
        }
        catch (const string& ex) {
            if (ex == noKey) {
                Method = response;
                Params.clear();
                Params.emplace_back(MethodTypeNames[response], FindFirst("result", envelope));
            }
            else {
                throw ex;
            }
        }

        Id = atoi(FindFirst(declareId, envelope).c_str());
    }

    string SerializeParameters(const vector<param>& attributes) {
        string output = "{";
        for (size_t i = 0; i < attributes.size(); ++i) {
            if (i) output += ",";
            const param& e = attributes[i];
            output += "\"" + e.Key + "\":" + e.Value;
        }
        output += "}";

        return output;
    }

public:
    RpcHandler() = default;
    ~RpcHandler() = default;

    string Serialize(Message data) {
        Method = data.Method;
        return Wrap(Method != response ? SerializeParameters(data.toVector()) : data.ResponseMessage);
    }

    Message Deserialize(const string& input) {
        UnWrap(input);
        switch (Method) {
            case loadParams: return LoadParamsMessage(FindFirst("MessageContent", Params));
            case response: return Response(FindFirst(MethodTypeNames[response], Params));
            default: return Message(Method);
        };
    }
};

#endif