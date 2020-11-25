

main : Main.o LinuxSupport.o LinuxSocketSupport.o
		g++ -g -std=c++11 -Wall -Wextra -Werror -o main Main.o LinuxSupport.o LinuxSocketSupport.o -I.

Main.o : Main.cpp Support.h SocketSupport.h
		g++ -g -c -std=c++11 -Wall -Wextra -Werror Main.cpp -I.

LinuxSupport.o : LinuxSupport.cpp Support.h
		g++ -g -c -std=c++11 -Wall -Wextra -Werror LinuxSupport.cpp -I.

LinuxSocketSupport.o : LinuxSocketSupport.cpp SocketSupport.h
		g++ -g -c -std=c++11 -Wall -Wextra -Werror LinuxSocketSupport.cpp -I.

zap :
		rm main *.o




