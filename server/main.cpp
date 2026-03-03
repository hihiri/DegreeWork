#include "common.h"
#include "TcpHandler.hpp"
#include "ServerApp.hpp"
#include <iostream>

int main(){
    TcpHandler srv(PORT);
    if(!srv.listen()) return 1;
    std::cout<<"Server listening on port "<<PORT<<"\n";
    if(!srv.acceptClient()) return 1;
    std::cout<<"Client connected\n";
    ServerApp app;
    app.run(srv);
    srv.closeClient();
    srv.closeListen();
    std::cout<<"Server shutting down\n";
    return 0;
}
