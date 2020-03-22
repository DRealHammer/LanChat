#include "LanChat.hh"




LanChat::LanChat(int port, std::string ip)
: inChat(false), isRunning(true), own_port(port), own_ip(ip){
    // create tcp server socket to given port

    
    this->serverSocket.bindTCP(ip, port);
    

    // set the serverSocket to non blocking, so it can jump over the accept() in the
    // mainloop

    // let the socket listen
    this->serverSocket.listenTCP(MAX_QUEUE);
    
}

void LanChat::mainloop(){

    while(isRunning){

        this->listener = std::thread(&LanChat::acceptConnections, this);
        this->commander =std::thread(&LanChat::console, this);
            

        this->listener.join();
        this->commander.join();

    }
}

void LanChat::console(){


    std::string input;
    std::cout << "> ";
    std::cin >> input;

    std::string argument;

    if (input == "/exit"){
        this->isRunning = false;
        serverSocket.closeTCP();

    }else if (input == "/connect"){
        // open new terminal window
        // start new socket connection to the given ip
        
        std::cout << "connect eingegeben" << std::endl;
        if(inChat){
            std::cout << "You're already in a chat, to connect to another user first disconnect ('/disconnect')" << std::endl;
            std::cin.ignore(INT_MAX);
        } else {
            std::cout << "enter ip, port" << std::endl;
            int port;
            std::cin >> argument;
            std::cin >> port;
            //maybe thread it later
            std::cout << "start establishConnections() to:" << std::endl;
            std::cout << "port " << port << std::endl;
            std::cout << "ip " << argument << std::endl;
            establishConnection(argument, port);
        }

    }else if(input == "/disconnect"){
        if(!inChat){
            std::cout << "You're currently not in a chat ('/connect <ip> <port>' to connect, '/exit' to quit" << std::endl;

        }else{
            sendSocket.closeTCP();
            readSocket.closeTCP();
            inChat = false;
        }
    }

    // eine nachricht




    
}

void LanChat::acceptConnections(){

    std::cout << "thread with acceptConnections() started" << std::endl;

    if(this->inChat){
        
        return;
    }

    this->readSocket = serverSocket.acceptTCP();

    if(!isRunning){
        //the accept ended because the socket was closed
        return;
    }

    if(inChat){
        return;
    }

    std::cout << "accepted an incoming connection" << std::endl;


    // setting a flag, so /connect can't make problems, while we establish the full connection
    this->inChat = true;
    std::cout << "set inChat to: " << inChat << std::endl;

    

    // get the port of the other user
    std::string other_port = readSocket.recvTCP();
    std::cout << "partner port: " << other_port << std::endl;

    int port = std::stoi(other_port);

    // connect to other user with the port he transmitted

    sendSocket.connectTCP(readSocket.getIP(), readSocket.getPort());


    std::cout << "both connections established, ready to send and receive" << std::endl;
    std::cout << "end of acceptConnections()" << std::endl;

}

void LanChat::establishConnection(std::string ipaddress, int port){

    if(inChat){
        return;
    }

    sendSocket.connectTCP(ipaddress, port);


    std::cout << "connect war erfolgreich, sende eigenen port" << std::endl;
    // could connect to the other user


    


    this->inChat = true;
    // to let the listener thread end
    serverSocket.closeTCP();

    // open it, to get a new socket for the current process
    serverSocket.openTCP();

    // bind it again for the other user
    serverSocket.bindTCP(this->own_ip, this->own_port);

    //get the fricken port as chars
    std::string own_p = std::to_string(this->own_port);
    //send the port of own server for bidirectional communication
    sendSocket.sendTCP(own_p);

    std::cout << "eigner port wurde übermittelt" <<std::endl;
    acceptConnection();
    std::cout << "der andere hat eine verbindung zum eigenen server aufgebaut" << std::endl;

}

void LanChat::acceptConnection(){


    this->readSocket = serverSocket.acceptTCP();
}

LanChat::~LanChat(){
    serverSocket.closeTCP();
    sendSocket.closeTCP();
    readSocket.closeTCP();
}

