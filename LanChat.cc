#include "LanChat.hh"




LanChat::LanChat(int port, std::string ip)
: inChat(false), isRunning(true), serverSocket(0), sendSocket(0), readSocket(0), own_port(port){
    // create tcp server socket to given port

    if((this->serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cerr << "socket creation failed" << std::endl;;
    }
    this->own_addrlen = sizeof(this->own_addr);

    this->own_addr.sin_family = AF_INET;

    inet_aton(ip.c_str(), &own_addr.sin_addr);
    

    own_addr.sin_port = htons(port);

    if(bind(serverSocket, (struct sockaddr *) &own_addr, sizeof(this->own_addr)) == -1){
        std::cerr << "binding of socket failed" << std::endl;
        perror(NULL);
    }

    // set the serverSocket to non blocking, so it can jump over the accept() in the
    // mainloop

    // let the socket listen
    if (listen(serverSocket, 2) == -1){
            // error bei listen
            std::cerr << "listen failed" << std::endl;
            perror(NULL);
    }
    
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
        close(this->serverSocket);

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
            close(this->sendSocket);
            close(this->readSocket);
            inChat = false;
            
        }
    }

    // eine nachricht




    
}

void LanChat::acceptConnections(){

    std::cout << "thread with acceptConnections() started" << std::endl;
    sockaddr_in incoming;
    socklen_t incoming_len = sizeof(incoming);

    while(isRunning){

        if(this->inChat){
            
            return;
        }

        this->readSocket = accept(serverSocket, (sockaddr*) &incoming, &incoming_len);

        if(!isRunning){
            //the accept ended because the socket was closed
            return;
        }

        if(inChat){
            return;
        }

        std::cout << "accepted an incoming connection" << std::endl;

        // failed
        if( this->readSocket == -1){
            std::cerr << "failed to create connection to client (accept)" << std::endl;
            perror(NULL);
            
        } else{
            break;
        }
    }

    // setting a flag, so /connect can't make problems, while we establish the full connection
    this->inChat = true;
    std::cout << "set inChat to: " << inChat << std::endl;


    this->cli_addr = incoming;
    this->cli_addrlen = incoming_len;

    // create new socket to write/send to other user

    if((this->sendSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror(NULL);
        return;
    }

    char other_port[5];
    memset(other_port, 0, 5);
    // get the port of the other user
    read(this->readSocket, other_port, sizeof(char)*5);
    std::cout << "partner port: " << other_port << std::endl;

    int port = std::atoi(other_port);

    // connect to other user with the port he transmitted

    sockaddr_in outgoing;
    outgoing.sin_family = cli_addr.sin_family;
    outgoing.sin_addr = cli_addr.sin_addr;
    outgoing.sin_port = htons(port);
    outgoing.sin_len = cli_addrlen;

    if(connect(this->sendSocket, (sockaddr*)&outgoing, outgoing.sin_len)){
        perror(NULL);
        return;
    }

    
    std::cout << "both connections established, ready to send and receive" << std::endl;
    std::cout << "end of acceptConnections()" << std::endl;

    
}

void LanChat::establishConnection(std::string ipaddress, int port){

    if(inChat){
        return;
    }
    
    if((this->sendSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror(NULL);
    }

    std::cout << "send socket erstellt" << std::endl;

    this->cli_addr.sin_family = AF_INET;
    inet_aton(ipaddress.c_str(), &this->cli_addr.sin_addr);
    this->cli_addr.sin_port = htons(port);
    socklen_t s = sizeof(this->cli_addr);

    this->cli_addrlen = sizeof(this->cli_addrlen);


    if (connect(this->sendSocket, (sockaddr *) &this->cli_addr, s) == -1){
        perror(NULL);
        return;
    }

    std::cout << "connect war erfolgreich, sende eigenen port" << std::endl;
    // could connect to the other user


    //send the port of own server for bidirectional communication

    //get the fricken port as chars
    std::string p = std::to_string(this->own_port);

    this->inChat = true;
    // to let the listener thread end
    close(this->serverSocket);

    // open it, to get a new socket for the current process
    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    bind(this->serverSocket, (sockaddr*) &this->own_addr, this->own_addrlen);
    write(sendSocket, p.c_str(), p.size()*sizeof(char));

    std::cout << "eigner port wurde übermittelt" <<std::endl;
    acceptConnection();
    std::cout << "der andere hat eine verbindung zum eigenen server aufgebaut" << std::endl;

}

void LanChat::acceptConnection(){

    sockaddr_in new_client;
    socklen_t new_clilen;


    this->readSocket = accept(this->serverSocket, (sockaddr *) &new_client, &new_clilen);
}

LanChat::~LanChat(){
    
    close(this->serverSocket);
    if(inChat){
        close(this->sendSocket);
        close(this->readSocket);
    }
}

