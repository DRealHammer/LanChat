#include <iostream>
#include <vector>

#define VERSION 0.1
#define PORT 50000
#define NO_PORT 1
#define INVALID_PORT 2
#define ILLEGAL_PARAMETER 3
#define NO_IP_GIVEN 4

#include "LanChat.hh"


int main(int argc, char** argv){

    int port = PORT;
    std::string ip = "";

    std::vector<std::string> arguments;
    arguments.resize(argc);

    // überspringe pfad
    for (int i = 1; i < argc; i++){
        arguments[i] = std::string(argv[i]);
        if ( arguments[i] == "-v" || arguments[i] == "--version"){
            std::cout << "LanChat version " << VERSION << std::endl;
            return 0;
        }

        else if(arguments[i] == "-p"){
            if (i+1 >= arguments.size()){
                std::cout << "usage: ./<name> -a <ip> -p <port>" << std::endl;
                return NO_PORT;
            } else{
                try{
                    port = std::atoi(argv[i+1]);
                } catch (...){
                    std::cerr << argv[i+1] << "is not a valid port" << std::endl;
                    return INVALID_PORT;
                }
            }
        }

        else if(arguments[i] == "-a"){
            if (i+1 >= arguments.size()){
                std::cout << "usage: ./<name> -a <ip> -p <port>" << std::endl;
                return NO_PORT;
            } else{
                ip = arguments[i+1];
            }
        }
        else if(arguments[i] == "-h" || arguments[i] == "--help"){
            std::cout << "-h | --help\tto show this menu" << std::endl;
            std::cout << "-v | --version\tto show the current version" << std::endl;
            std::cout << "-a <ip>\t\tto give your local ip (else it runs on 172.0.0.1)" << std::endl;
            std::cout << "-p <port>\tto give the port you want LanChat to run on" << std::endl;
            return 0;
        }

    }

    if (ip == ""){
        return NO_IP_GIVEN;
    }
    LanChat chat(port, ip);
    chat.mainloop();
    return 0;
}
