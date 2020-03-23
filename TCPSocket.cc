#include "TCPSocket.hh"



#define BUFFER 256

TCPSocket::TCPSocket():isConnected(false), isListening(false){

    // constructor for the user


    int testSocket = socket(AF_INET, SOCK_STREAM, 0);

    // failed
    if(testSocket == -1){
        std::cerr << "failed in creating a socket" << std::endl;
        perror(NULL);
        return;
    }

    // socket was created
    this->s = testSocket;
    this->isOpen = true;


}

TCPSocket::TCPSocket(int _socket, sockaddr_in _address, socklen_t _addr_len, std::string _ip, int _port)
: s(_socket), isConnected(true), isListening(false), isOpen(true), address_str(_ip), port(_port)
{
    // constructor for the accept() function
    // hence connected is true, because it comes from the server socket
    // and of course not listening

    this->address = _address;
    this->addr_len = _addr_len;

}

void TCPSocket::openTCP(){
    if (this->isOpen){
        std::cerr << "already open" << std::endl;
        perror(NULL);
        return;
    }

    int testSocket = socket(AF_INET, SOCK_STREAM, 0);

    // failed
    if(testSocket == -1){
        std::cerr << "failed in opening a socket" << std::endl;
        perror(NULL);
        return;
    }

    // socket was created
    this->s = testSocket;
    this->isOpen = true;
}

void TCPSocket::connectTCP(std::string _ip, int _port){

    if(this->isListening){
        std::cerr << "this socket is already listening" << std::endl;
        perror(NULL);
        return;
    }

    if(this->isConnected){
        std::cerr << "this socket is already connected" << std::endl;
        perror(NULL);
        return;
    }

    this->address_str = _ip;
    this->port = _port;

    this->address.sin_family = AF_INET;
    inet_aton(_ip.c_str(), &this->address.sin_addr);
    this->address.sin_port = htons(_port);

    this->addr_len = sizeof(this->address);

    int testConnect = connect(this->s, (sockaddr*) &this->address, this->addr_len);
    if (testConnect == -1){
        std::cerr <<("failed to connect to socket") << std::endl;
        perror(NULL);
    }

    this->isConnected = true;
}

void TCPSocket::bindTCP(std::string _ip, int _port){
    this->address_str = _ip;
    this->port = _port;

    this->address.sin_family = AF_INET;
    inet_pton(AF_INET, _ip.c_str(), &this->address.sin_addr);
    this->address.sin_port = htons(_port);
    this->addr_len = sizeof(this->address);

    int testBind = bind(this->s, (sockaddr*) &this->address, this->addr_len);
    if (testBind == -1){
        std::cerr << "failed to bind socket" << std::endl;
        perror(NULL);
    }

}

void TCPSocket::listenTCP(int _maxQueue){

    if(this->isConnected){
        std::cerr << "this socket is already connected" << std::endl;
        perror(NULL);
    }

    if(this->isListening){
        std::cerr << "this socket is already listening" << std::endl;
        perror(NULL);
    }



    int listenTest = listen(this->s, _maxQueue);
    if (listenTest == -1){
        std::cerr << "failed to start listening of socket" << std::endl;
        perror(NULL);
        return;
    }
    this->isListening = true;


}

void TCPSocket::operator=(const TCPSocket& o){
    if (this->isOpen){
        this->closeTCP();
    }
    this->s = o.s;
    this->isOpen = o.isOpen;
    this->isConnected = o.isConnected;
    this->isListening = o.isListening;

    this->address_str = o.address_str;
    this->port = o.port;
    this->address = o.address;
    this->addr_len = o.addr_len;
}


TCPSocket TCPSocket::acceptTCP(int blocking){

    bool _blocking;
    if( blocking == BLOCKING){
        _blocking = true;
    }
    if(blocking == NOT_BLOCKING){
        _blocking = false;
    }
    
    if(!this->isListening){
        std::cerr << "this socket is not listening" << std::endl;
        perror(NULL);
    }

    if(this->isConnected){
        std::cerr << "this socket is already connected" << std::endl;
        perror(NULL);
    }


    sockaddr_in testAddress;
    socklen_t testLen;

    if(!_blocking){
        int flags = fcntl(this->s, F_GETFL);
        fcntl(this->s, F_SETFL, (O_NONBLOCK | flags ));
    }

    int acceptTest = accept(this->s, (sockaddr*) &testAddress, &testLen);


    if (acceptTest == -1){
        if (_blocking){
            std::cerr << "accept failed" << std::endl;
            perror(NULL);
        }else{
            if( errno == EWOULDBLOCK || errno == EAGAIN){
                throw NO_CONNECTION;
            } else{
                std::cerr << "accept failed" << std::endl;
                perror(NULL);
            }
        }
        
    } else{

        // we have a new socket in acceptTest
        char addr_buf[30];
        inet_ntop(AF_INET, &testAddress, addr_buf, testLen);
        std::string addr(addr_buf);

        int port = ntohs(testAddress.sin_port);

        return TCPSocket(acceptTest, testAddress, testLen, addr, port);
    }
    return TCPSocket();
}

void TCPSocket::sendTCP(std::string message){
    if(message.size() > BUFFER){
        std::cerr << "to big message to send" << std::endl;
        perror(NULL);
    }
    size_t size = message.size() * sizeof(char);
    int testWrite = write(this->s, message.c_str(), size);
    if (testWrite == -1){
        std::cerr << "sending of \"" + message + "\" failed" << std::endl;
        perror(NULL);
    }
}

std::string TCPSocket::recvTCP(){
    // static, only once created for the method recvTCP
    static char buffer[BUFFER];
    memset(buffer, 0, BUFFER);
    int readTest = read(this->s, buffer, BUFFER*sizeof(char));
    if (readTest <= 0){
        std::cerr << "failed to read from socket" << std::endl;
        perror(NULL);
        return "";
    }

    return std::string(buffer);
    
}

void TCPSocket::closeTCP(){
    if(this->isOpen){
        close(this->s);
        this->isOpen = false;
    }
}

std::string TCPSocket::getIP() const{
    return this->address_str;
}

int TCPSocket::getPort() const{
    return this->port;
}

bool TCPSocket::getOpen() const{
    return this->isOpen;
}
bool TCPSocket::getConnected() const{
    return this->isConnected;
}
bool TCPSocket::getListening() const{
    return this->isListening;
}

TCPSocket::~TCPSocket(){
    if(this->isOpen){
        close(this->s);
    }
}