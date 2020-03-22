#ifndef LANCHAT_SOCKET
#define LANCHAT_SOCKET


#include <string>
#include <exception>

// for sockets
#include <sys/socket.h>

// addr struct
#include <netinet/in.h>

// for write/read
#include <unistd.h>

// internet
#include <arpa/inet.h>




class TCPSocket{

private:

    int s;
    bool isOpen;
    bool isConnected;
    bool isListening;

    sockaddr_in address;
    socklen_t addr_len;

    TCPSocket(int _socket, sockaddr_in _address, socklen_t _addr_len);


public:

    TCPSocket();
    void connectTCP(std::string _ip, int _port);
    void bindTCP(std::string _ip, int _port);
    void listenTCP(int _maxQueue);
    TCPSocket acceptTCP();
    void sendTCP(std::string _message);
    std::string recvTCP();
    ~TCPSocket();

};





#endif // LANCHAT_SOCKET