#include "RpcHandler.hpp"
#include "NetworkHandler.hpp"

using namespace std;

int main() {
    
    auto networkHandler = new ClientNetworkHandler();    

    RpcHandler* rpcHandler = new RpcHandler();

    networkHandler->Send(rpcHandler->Serialize(*new Message(startCalculation)).c_str());

    //receiving messages
    rpcHandler->Deserialize(networkHandler->WaitReceive());

    string s;
    cin >> s;
    return 0;
}