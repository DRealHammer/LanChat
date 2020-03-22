#include "TCPSocket.hh"


#define BUFFER 256

TCPSocket::TCPSocket():isConnected(false), isListening(false){

    // constructor for the user


    int testSocket = socket(AF_INET, SOCK_STREAM, 0);

    // failed
    if(testSocket == -1){
        throw "failed in creating a socket";
    }

    // socket was created
    this->s = testSocket;
    this->isOpen = true;


}

TCPSocket::TCPSocket(int _socket, sockaddr_in _address, socklen_t _addr_len)
: s(_socket), isConnected(true), isListening(false), isOpen(true)
{
    // constructor for the accept() function
    // hence connected is true, because it comes from the server socket
    // and of course not listening

    this->address = _address;
    this->addr_len = _addr_len;
    
}


void TCPSocket::connectTCP(std::string _ip, int _port){

    if(this->isListening){
        throw "this socket is already listening";
    }

    if(this->isConnected){
        throw "this socket is already connected";
    }

    this->address.sin_family = AF_INET;
    inet_aton(_ip.c_str(), &this->address.sin_addr);
    this->address.sin_port = htons(_port);

    int testConnect = connect(this->s, (sockaddr*) &this->address, this->addr_len);
    if (testConnect == -1){
        throw("failed to connect to socket");
    }

    this->isConnected = true;
}

void TCPSocket::bindTCP(std::string _ip, int _port){
    this->address.sin_family = AF_INET;
    inet_aton(_ip.c_str(), &this->address.sin_addr);
    this->address.sin_port = htons(_port);

    int testBind = bind(this->s, (sockaddr*) &this->address, this->addr_len);
    if (testBind == -1){
        throw "failed to bind socket";
    }

}

void TCPSocket::listenTCP(int _maxQueue){

    if(this->isConnected){
        throw "this socket is already connected";
    }

    if(this->isListening){
        throw "this socket is already listening";
    }



    int listenTest = listen(this->s, _maxQueue);
    if (listenTest == -1){
        throw "failed to start listening of socket";
    }
}


TCPSocket TCPSocket::acceptTCP(){

    if(!this->isListening){
        throw "this socket is not listening";
    }

    if(this->isConnected){
        throw "this socket is already connected";
    }

    sockaddr_in testAddress;
    socklen_t testLen;

    int acceptTest = accept(this->s, (sockaddr*) &testAddress, &testLen);
    if (acceptTest == -1){
        throw "failed to accept a connection";
    }

    // we have a new socket in acceptTest

    return TCPSocket(acceptTest, testAddress, testLen);

}

void TCPSocket::sendTCP(std::string message){
    if(message.size() > BUFFER){
        throw "to big message to send";
    }
    size_t size = message.size() * sizeof(char);
    int testWrite = write(this->s, message.c_str(), size);
    if (testWrite == -1){
        throw "sending of \"" + message + "\" failed";
    }
}

std::string TCPSocket::recvTCP(){
    // static, only once created for the method recvTCP
    static char buffer[BUFFER];
    memset(buffer, 0, BUFFER);
    int readTest = read(this->s, buffer, BUFFER*sizeof(char));
    if (readTest >= 0){
        throw "failed to read from socket";
    }

    return std::string(buffer);
    
}

TCPSocket::~TCPSocket(){
    if(this->isOpen){
        close(this->s);
    }
}