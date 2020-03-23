LanChat: main.o LanChat.o TCPSocket.o
			g++ -o LanChat main.o LanChat.o TCPSocket.o
			rm main.o
			rm LanChat.o
			rm TCPSocket.o

main.o: main.cc
			g++ -c -std=c++11 main.cc

LanChat.o: LanChat.cc
			g++ -c -std=c++11 LanChat.cc
TCPSocket.o: TCPSocket.cc
			g++ -c -std=c++11 TCPSocket.cc