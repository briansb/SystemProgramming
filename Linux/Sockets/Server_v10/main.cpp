// https://tangentsoft.net/wskfaq/
// https://www.studytonight.com/network-programming-in-python/working-with-tcp-sockets
// excellent resources for both client and server

#include "Support.h"
#include "SocketSupport.h"
#include <thread>

bool keep_running = true;
std::string strMessageToServer;
std::string strMessageFromServer;


int main()
{
	// set keyboard handler
	winuxSetKeyboardHandler();

	if (InitializeSockets())
		std::cout << "Sockets initialized" << std::endl;
	else { std::cout << "Could not initialize sockets" << std::endl; exit(1); }

	if (CreateSocket())
		std::cout << "Socket created" << std::endl;
	else { std::cout << "Could not create socket" << std::endl; SocketShutdown(); exit(1); }

	if (BindToSocket())
		std::cout << "Socket bind successful" << std::endl;
	else { std::cout << "Could not bind to socket" << std::endl; SocketShutdown(); exit(1); }

	if (ListenOnSocket())
		std::cout << "Listening..." << std::endl;
	else { std::cout << "Can't hear a thing" << std::endl; SocketShutdown(); exit(1); }
	
	while (keep_running) {
		int client_sock = AcceptConnection();
		if (client_sock < 0) {
			std::cout << "Did not accept connection" << std::endl;
		}
		else {
			std::cout << "Accepted connection with client socket " << client_sock << std::endl;
			if (StartThread(client_sock)) {}
			else std::cout << "Could not start thread" << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
	
	if (SocketShutdown()) {
		std::cout << "Normal shutdown.  Press Enter to exit" << std::endl;
	}
	else {
		std::cout << "Abnormal shutdown.  Press Enter to exit" << std::endl;
	}

	std::cin.get();

	return 0;
}


