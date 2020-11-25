

main : main.o LinuxSupport.o LinuxSocketSupport.o
		g++ -g -std=c++11 -Wall -Wextra -Werror -o main main.o LinuxSupport.o LinuxSocketSupport.o -I. -pthread

main.o : main.cpp Support.h SocketSupport.h
		g++ -g -c -std=c++11 -Wall -Wextra -Werror main.cpp -I.

LinuxSupport.o : LinuxSupport.cpp Support.h
		g++ -g -c -std=c++11 -Wall -Wextra -Werror LinuxSupport.cpp -I.

LinuxSocketSupport.o : LinuxSocketSupport.cpp SocketSupport.h
		g++ -g -c -std=c++11 -Wall -Wextra -Werror LinuxSocketSupport.cpp -I.

zap :
		rm main *.o




