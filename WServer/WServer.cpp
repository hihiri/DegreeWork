#include <tchar.h>
#include "RpcHandler.hpp"
#include "NetworkHandler.hpp"

using namespace std;

int main() {    

    ServerNetworkHandler* networkHandler = new ServerNetworkHandler();

    RpcHandler* rpcHandler = new RpcHandler();
    Message result = rpcHandler->Deserialize(networkHandler->WaitReceiving());

    networkHandler->Send({ });
    
    return 0;
}