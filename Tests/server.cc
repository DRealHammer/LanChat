#include "TCPSocket.hh"
#include <unistd.h>
#include <iostream>

int main(){

        TCPSocket server;
        server.bindTCP("192.168.178.38", 50000);
        server.listenTCP(2);
        TCPSocket client = server.acceptTCP();
        std::cout << client.recvTCP() << std::endl;
        client.sendTCP("1 und 1 macht zwei du schlumpf, das hier wird ein sehr großer test...");
        
    
}