// https://docs.microsoft.com/en-us/windows/win32/winsock/winsock-functions

#include "Support.h"
#include "SocketSupport.h"
#include <thread>

bool keep_running = true;
std::string strMessageToServer;
std::string strMessageFromServer;

int main(int argc, char* argv[])
{
	// seed the random number generator
	srand((int)time(0));
	
	// set keyboard handler
	winuxSetKeyboardHandler();

	
	if (InitializeSockets())
		std::cout << "Sockets initialized" << std::endl;
	else { std::cout << "Could not initialize sockets" << std::endl; exit(1); }

	if (CreateSocket())
		std::cout << "Socket created" << std::endl;
	else { std::cout << "Could not create socket" << std::endl; SocketShutdown(); exit(1); }
	
	if (ConnectToSocket())
		std::cout << "Connected to socket" << std::endl;
	else { std::cout << "Could not connect to socket" << std::endl; SocketShutdown(); exit(1); }


	while (keep_running) {
		// define a variable message to send to server
		int n = rand() % 1000 + 1;
		strMessageToServer = "Client PID = " + std::to_string(GetCurrentProcessId()) + " client data = " + std::to_string(n);
		if (SendToServer(strMessageToServer)) { 
			std::cout << "Sent to server:  " << strMessageToServer << std::endl; 
		} 
		else { 
			std::cout << "Could not send to server" << std::endl; 
			break;
		}
		
		if (ReceiveFromServer(strMessageFromServer)) {
			std::cout << "Received from server:  " << strMessageFromServer << std::endl;
		}
		else {
			std::cout << "Could not receive from server" << std::endl;
			break;
		}


		// this is the preferred method
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



