#include <tchar.h>
#include "RpcHandler.hpp"
#include "NetworkHandler.hpp"

using namespace std;

int main() {    

    auto networkHandler = new ServerNetworkHandler();

    RpcHandler* rpcHandler = new RpcHandler();
    Message result = rpcHandler->Deserialize(networkHandler->WaitReceive());

    networkHandler->Send(rpcHandler->Serialize(result));
    
    return 0;
}