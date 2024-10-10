#include "RpcHandler.hpp"
#include "NetworkHandler.hpp"

using namespace std;

int main() {
    
    auto networkHandler = new ClientNetworkHandler();    

    RpcHandler* rpcHandler = new RpcHandler();

    networkHandler->Send(rpcHandler->Serialize(*new LoadParamsMessage("6789123456")));

    //receiving messages
    rpcHandler->Deserialize(networkHandler->WaitReceive());
    return 0;
}