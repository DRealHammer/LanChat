#ifndef LANCHAT
#define LANCHAT

#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <functional>


// sockets
#include <sys/socket.h>

// addr struct
#include <netinet/in.h>

// for write/read
#include <unistd.h>

// internet
#include <arpa/inet.h>

// for timeout when checking if somebody tries to connect
#include <sys/time.h>



class LanChat{

public:

    // open tcp socket for incoming connections
    LanChat(int port, std::string ip);

    // asks the user for input and provides an interface for the chat.
    void mainloop();

    ~LanChat();
    
private:

    int serverSocket, sendSocket, readSocket;
    int own_port;

    bool inChat, isRunning;

    sockaddr_in own_addr;
    socklen_t own_addrlen;

    sockaddr_in cli_addr;
    socklen_t cli_addrlen;

    std::thread listener;
    std::thread commander;

    // receives the new created socket
    void establishConnection(std::string ip, int port);

    void acceptConnections();
    void acceptConnection();

    void console();

    
};




#endif // LANCHAT
