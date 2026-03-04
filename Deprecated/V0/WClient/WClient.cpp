#include "RpcHandler.hpp"
#include "NetworkHandler.hpp"

using namespace std;

int main() {
    
    ClientNetworkHandler networkHandler;

    RpcHandler rpcHandler;

    Message msg(startCalculation);
    string serialized = rpcHandler.Serialize(msg);
    networkHandler.Send(serialized.c_str());

    // receiving messages
    rpcHandler.Deserialize(networkHandler.WaitReceive());

    string s;
    cin >> s;
    return 0;
}