#include <tchar.h>
#include "RpcHandler.hpp"
#include "NetworkHandler.hpp"

using namespace std;

int main() {    

    auto networkHandler = new ServerNetworkHandler();

    RpcHandler* rpcHandler = new RpcHandler();

    string message = networkHandler->WaitReceive();

    Response response = Response("\"ok\"");
    try {
        Message result = rpcHandler->Deserialize(message);
    }
    catch (string ex) {
        response = Response(ex);
    }

    auto dummy = rpcHandler->Serialize(response);
    networkHandler->Send(dummy.c_str());
    
    return 0;
}