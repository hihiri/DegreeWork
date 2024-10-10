#include "RpcHandler.hpp"
#include "NetworkHandler.hpp"

using namespace std;

int main() {
    
    ClientNetworkHandler* networkHandler = new ClientNetworkHandler();
    

    RpcHandler* rpcHandler = new RpcHandler();

    networkHandler->Send(rpcHandler->Serialize(*new LoadParamsMessage("6789123456")).c_str());

    //receiving messages
    rpcHandler->Deserialize(networkHandler->WaitReceiving());
    return 0;
}