#ifndef LANCHAT_SOCKET
#define LANCHAT_SOCKET


#include <string>
#include <exception>
#include <iostream>

// for sockets
#include <sys/socket.h>

// addr struct
#include <netinet/in.h>

// for write/read
#include <unistd.h>

// internet
#include <arpa/inet.h>

#include <fcntl.h>


#define NO_CONNECTION 1

#define BLOCKING 0
#define NOT_BLOCKING 1




class TCPSocket{

private:

    int s;
    bool isOpen;
    bool isConnected;
    bool isListening;
    bool isAccepting;

    std::string address_str;
    int port;
    sockaddr_in address;
    socklen_t addr_len;

    TCPSocket(int _socket, sockaddr_in _address, socklen_t _addr_len, std::string _ip, int _port);


public:

    TCPSocket();
    void openTCP();
    void connectTCP(std::string _ip, int _port);
    void bindTCP(std::string _ip, int _port);
    void listenTCP(int _maxQueue);

    void operator=(const TCPSocket& o);

    // if blocking is disabled, the socket will throw a NO_CONNECTION exception (int)
    TCPSocket acceptTCP(int _blocking);
    void sendTCP(std::string _message);
    std::string recvTCP();


    void closeTCP();

    std::string getIP() const;
    int getPort() const;
    bool getOpen() const;
    bool getConnected() const;
    bool getListening() const;
    bool getAccepting() const;

    ~TCPSocket();

};





#endif // LANCHAT_SOCKET