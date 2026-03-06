#include <tchar.h>
#include "RpcHandler.hpp"
#include "NetworkHandler.hpp"

using namespace std;

int main() {    

    ServerNetworkHandler networkHandler;

    RpcHandler rpcHandler;

    string message = networkHandler.WaitReceive();

    Response response("\"ok\"");
    try {
        Message result = rpcHandler.Deserialize(message);
    }
    catch (const string& ex) {
        response = Response(ex);
    }

    auto dummy = rpcHandler.Serialize(response);
    networkHandler.Send(dummy.c_str());

    return 0;
}