#include "../TCPSocket.hh"
#include <unistd.h>
#include <iostream>

int main(){

        TCPSocket server;
        server.bindTCP("192.168.178.38", 50000);
        server.listenTCP(2);
        TCPSocket client = server.acceptTCP();
        std::cout << client.getIP() << std::endl;
        
    
}