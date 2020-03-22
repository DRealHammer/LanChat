LanChat: main.o LanChat.o
			g++ -o LanChat main.o LanChat.o
			rm main.o
			rm LanChat.o

main.o: main.cc
			g++ -c -std=c++11 main.cc

LanChat.o: LanChat.cc
				g++ -c -std=c++11 LanChat.cc