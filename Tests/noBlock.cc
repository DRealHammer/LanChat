#include "../TCPSocket.hh"
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

int main(){
    

    if(fork() == 0){
        // client
        TCPSocket sender;
        std::cin.get();
        sender.connectTCP("192.168.178.38", 50000);
        sender.closeTCP();

    } else{
        int s = socket(AF_INET, SOCK_STREAM, 0);
        perror("nach socket(): ");
        

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(50000);
        inet_pton(AF_INET, "192.168.178.38", &addr.sin_addr);
        socklen_t addrlen = sizeof(addr);

        bind(s, (sockaddr*) &addr, addrlen);
        perror("nach bind(): ");
        listen(s, 2);
        perror("nach listen");
        int flags = fcntl(s, F_GETFL);
        fcntl(s, F_SETFL, O_NONBLOCK | flags);
        sockaddr_in a;
        socklen_t l;
        while(true){
            std::cout << "accepting..." << std::endl;
            int c = accept(s, (sockaddr*) &a, &l);
            if(errno == EWOULDBLOCK || errno == EAGAIN){
                std::cout << "nothing accepted" << std::endl;
                sleep(1);
            }else{
                std::cout << "accepted" <<std::endl;
                return 0;
            }
            
        }
    }
    


}