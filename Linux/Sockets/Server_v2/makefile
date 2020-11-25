

main : Main.o LinuxSocketSupport.o
		g++ -g -std=c++11 -Wall -Wextra -Werror -o main Main.o LinuxSocketSupport.o -I.

Main.o : Main.cpp SocketSupport.h
		g++ -g -c -std=c++11 -Wall -Wextra -Werror Main.cpp -I.

LinuxSocketSupport : LinuxSocketSupport.h LinuxSocketSupport.cpp
		g++ -g -c -std=c++11 -Wall -Wextra -Werror LinuxSocketSupport.cpp -I.

zap :
		rm main *.o




