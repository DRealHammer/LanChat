#include <iostream>
#include "../TCPSocket.hh"

int main(){
    TCPSocket client;
    client.connectTCP("192.168.178.38", 50000);
    TCPSocket client2 = client;
    client2.sendTCP("die nachricht vom client sender");
    std::cout << client.recvTCP() << std::endl;

}