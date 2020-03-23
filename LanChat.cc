#include "LanChat.hh"




LanChat::LanChat(int port, std::string ip)
: inChat(false), isRunning(true), own_port(port), own_ip(ip), isAccepting(false){
    // create tcp server socket to given port

    this->serverSocket.bindTCP(ip, port);
    
    // set the serverSocket to non blocking, so it can jump over the accept() in the
    // mainloop

    // let the socket listen
    this->serverSocket.listenTCP(MAX_QUEUE);
    
}

void LanChat::mainloop(){

    while(this->isRunning){

        this->listener = std::thread(&LanChat::acceptConnections, this);
        this->commander = std::thread(&LanChat::console, this);
            

        this->listener.join();
        this->commander.join();

    }
}

void LanChat::console(){

    while(this->isRunning){
        std::string input;
        std::cout << "> ";
        std::cin >> input;

        std::string argument;

        if (input == "/exit"){
            this->isRunning = false;
            return;

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
                std::cout << "start establishConnections() on:" << std::endl;
                std::cout << "port " << port << std::endl;
                std::cout << "ip " << argument << std::endl;
                establishConnection(argument, port);
            }

        }else if(input == "/disconnect"){
            if(!inChat){
                std::cout << "You're currently not in a chat ('/connect <ip> <port>' to connect, '/exit' to quit" << std::endl;
                continue;
            }else{
                sendSocket.closeTCP();
                readSocket.closeTCP();
                inChat = false;
            }
        }else{
            //sendMessage();
        }
    }

}

void LanChat::acceptConnections(){

    if(this->isAccepting){
        return;
    }

    this->isAccepting = true;

    TCPSocket testAccept;
    // while(isRunning){

        // to check if we started a connection
        if(this->inChat){
            this->isAccepting = false;
            return;
        }

        this->readSocket = serverSocket.acceptTCP();
    /* the non blocking version
        try{
            testAccept = serverSocket.acceptTCP();
            break;
        } catch (int error){
            if(error == NO_CONNECTION){
                sleep(1);
                continue;
            }else{
                std::cout << error << std::endl;
                std::cerr << "could not accept" << std::endl;
                this->isAccepting = false;
                return;
            }
        }
        */
    //}
    
   
   this->isAccepting = false;

    if(!this->isRunning){
        return;
    }

    // this->readSocket = testAccept;
    std::cout << "accepted an incoming connection" << std::endl;


    // setting a flag, so /connect can't make problems, while we establish the full connection
    this->inChat = true;

    
    // get the port of the other user
    std::string other_port = this->readSocket.recvTCP();
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

    //get the fricken port as chars
    std::string own_p = std::to_string(this->own_port);

    //send the port of own server for bidirectional communication
    sendSocket.sendTCP(own_p);

    std::cout << "eigner port wurde übermittelt" << std::endl;
    acceptConnection();
    std::cout << "der andere hat eine verbindung zum eigenen server aufgebaut" << std::endl;

}

void LanChat::acceptConnection(){

/*
    if(isAccepting){
        return;
    }
*/
    this->isAccepting = true;

    this->readSocket = serverSocket.acceptTCP();

    this->isAccepting = false;
}

LanChat::~LanChat(){
    serverSocket.closeTCP();
    sendSocket.closeTCP();
    readSocket.closeTCP();
}

