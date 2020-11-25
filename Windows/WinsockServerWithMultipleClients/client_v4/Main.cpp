#include "Support.h"
#include "SocketSupport.h"
#include <thread>


#pragma comment (lib, "Ws2_32.lib")

bool keep_running = true;
std::string strMessageToServer;

int main(int argc, char* argv[])
{
	// seed the random number generator
	srand((int)time(0));
	
	// set keyboard handler
	zSetKeyboardHandler();

	zSocketInitializationAndStart();

	while (keep_running) {
		// define a variable message to send to server
		int n = rand() % 1000 + 1;
		strMessageToServer = "Client data = " + std::to_string(GetCurrentProcessId()) + " " + std::to_string(n);

		zSendToServer(strMessageToServer);
		zReceiveFromServer(strMessageToServer);

		// this is the preferred method
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}


	zSocketShutdown();

	std::cout << "Normal exit, keep_running = " << keep_running << std::endl;
	std::cout << "Press Enter to exit" << std::endl;
	std::cin.get();
    
	return 0;
}



