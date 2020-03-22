#include <iostream>
#include <map>

#define VERSION 0.1
#define PORT 50000

#include "LanChat.hh"

int main(int argc, char** argv){

    int port = PORT;

    if (argc > 1){
        // argument was given
        std::string arg(argv[1]);
        
        if ( arg == "-v" || arg == "--version"){
            std::cout << "LanChat version " << VERSION << std::endl;
            return 0;
        } 
        else if(arg == "-p"){
            if (argc != 3){
                std::cout << "usage: ./<name> -p <port>" << std::endl;
                return 1;
            } else{
                port = std::atoi(argv[2]);
            }
        }else {

        std::cerr << "illegal parameter: " << arg << std::endl;
        return 1;
        }

    }


    LanChat chat(port, std::string("192.168.178.38"));
    chat.mainloop();
    return 0;
}
