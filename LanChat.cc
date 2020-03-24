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
        this->reader = std::thread(&LanChat::recvMessages, this);
            

        this->reader.join();
        this->listener.join();
        this->commander.join();

    }
}

void LanChat::console(){

    while(this->isRunning){
        std::string input;
        if(inChat){
            std::cout << this->readSocket.getIP() << "@" << this->readSocket.getPort();
        }
        std::cout << "> ";
        std::cin >> input;

        std::string argument;

        if (input == "/exit"){
            this->isRunning = false;
            return;

        }else if (input == "/connect"){
            // open new terminal window
            // start new socket connection to the given ip
            
            if(inChat){
                std::cout << "You're already in a chat, to connect to another user first disconnect ('/disconnect')" << std::endl;
                std::cin.clear();
            } else {
                std::cout << "enter ip, port" << std::endl;
                int port;
                std::cin >> argument;
                std::cin >> port;
                //maybe thread it later
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
            std::string message;
            std::getline(std::cin, message);
            message = input + message;
            sendMessage(message);
        }
    }

}

void LanChat::acceptConnections(){

    if(this->isAccepting){
        return;
    }

    this->isAccepting = true;

    TCPSocket testAccept;
    this->serverSocket.setBlocking(false);
    while(isRunning){
        
        // to check if we started a connection
        if(this->inChat){
            this->isAccepting = false;
            return;
        }

        try{
            testAccept = serverSocket.acceptTCP();
            break;
        } catch (int error){
            if(error == NO_DATA){
                sleep(1);
                continue;
            }else{
                std::cout << error << std::endl;
                std::cerr << "could not accept" << std::endl;
                this->isAccepting = false;
                return;
            }
        }

    }
    
   
   this->isAccepting = false;

    if(!this->isRunning){
        return;
    }

    this->readSocket = testAccept;
    std::cout << "accepted an incoming connection" << std::endl;


    // setting a flag, so /connect can't make problems, while we establish the full connection
    this->inChat = true;
    

    
    // get the port of the other user

    // set to blocking for the messages of the other user
    this->readSocket.setBlocking(true);

    // get the others ip and send it back for error checking
    std::string other_ip = this->readSocket.recvTCP();
    this->readSocket.sendTCP(other_ip);

    // get the others port and send it back s.a
    std::string other_port = this->readSocket.recvTCP();
    this->readSocket.sendTCP(other_port);


    int port = std::stoi(other_port);

    // connect to other user with the port he transmitted

    sendSocket.connectTCP(other_ip, port);


}

void LanChat::establishConnection(std::string ipaddress, int port){

    if(inChat){
        return;
    }

    sendSocket.connectTCP(ipaddress, port);
    this->inChat = true;

    // could connect to the other user


    
    //get the fricken port as chars
    std::string own_p = std::to_string(this->own_port);

    //send the port of own server for bidirectional communication
    sendSocket.sendTCP(this->own_ip);
    std::string testIP = sendSocket.recvTCP();

    sendSocket.sendTCP(own_p);
    std::string testPort = sendSocket.recvTCP();

    acceptConnection();

}

void LanChat::acceptConnection(){

/*
    if(isAccepting){
        return;
    }
*/
    this->isAccepting = true;
    // set server to blocking to get the others users package
    this->serverSocket.setBlocking(true);
    this->readSocket = serverSocket.acceptTCP();

    this->isAccepting = false;
}

void LanChat::sendMessage(std::string message) const{
    sendSocket.sendTCP(message);
}

void LanChat::recvMessages(){
    this->readSocket.setBlocking(false);
    std::string msg;
    while(isRunning){
        if(!inChat){
            sleep(1);
            continue;
        }
        if(!this->readSocket.getConnected()){
            sleep(1);
            continue;
        }
        try{
            msg = this->readSocket.recvTCP();
        } catch (int e){
            if(e == NO_DATA){
                sleep(1);
                continue;
            }
            else{
                std::cerr << "failed to receive message" << std::endl;
                perror(NULL);
                return;
            }
        }
        std::cout << msg << std::endl;
    }
}

LanChat::~LanChat(){
    serverSocket.closeTCP();
    sendSocket.closeTCP();
    readSocket.closeTCP();
}

