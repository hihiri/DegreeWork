#include <iostream>
#include "RpcHandler.hpp"

int main()
{
	auto v = new TestMessage("test", "6789123456");

	auto r = new RpcHandler();
	cout << r->Serialize(*v);

	//new instance ensuring independency
	auto k = new RpcHandler();
	auto result = k->Deserialize(r->Serialize(*v));

	cout << "Done!";

	return 0;
}
