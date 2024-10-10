#include <tchar.h>
#include "RpcHandler.hpp"
#include "NetworkHandler.hpp"

using namespace std;

int main() {    

    auto networkHandler = new ServerNetworkHandler();

    RpcHandler* rpcHandler = new RpcHandler();

    string message = networkHandler->WaitReceive();
    Message result = rpcHandler->Deserialize(message);

    networkHandler->Send(rpcHandler->Serialize(result).c_str());
    
    return 0;
}