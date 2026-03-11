#include "common.h"
#include "TcpHandler.hpp"
#include "ServerApp.hpp"
#include <iostream>
#include <csignal>

volatile sig_atomic_t should_exit = 0;

void signal_handler(int signal) {
    should_exit = 1;
}

int main(){
    signal(SIGINT, signal_handler);

    TcpHandler srv(PORT);
    if(!srv.listen()) return 1;
    std::cout<<"Server listening on port "<<PORT<<"\n";
    ServerApp app;
    while(!should_exit){
        if(!srv.acceptClient()) break;
        std::cout<<"Client connected\n";
        app.run(srv);
        srv.closeClient();
        std::cout<<"Client disconnected\n";
    }
    srv.closeListen();
    std::cout<<"Server shutting down\n";
    return 0;
}
