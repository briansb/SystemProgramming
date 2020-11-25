#include "WinsockClient.h"
#include "Support.h"
#include <thread>
#include <chrono>

#pragma comment (lib, "Ws2_32.lib")

bool keep_running = true;

int main(int argc, char* argv[])
{

    // Start Winsock up
    WSAData wsaData;
	int nCode;
    if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) { std::cout << "WSAStartup() returned error code " << nCode << "." << 	std::endl; return 1; }

	const char* pcHost = "127.0.0.1";
	int nPort = 4242;
	// Find the server's address
	std::cout << "Looking up address..." << std::endl;
	u_long nRemoteAddress = LookupAddress(pcHost);
	if (nRemoteAddress == INADDR_NONE) { std::cout << WSAGetLastErrorMessage("lookup address") << std::endl; return 1; }
	in_addr Address;
	memcpy(&Address, &nRemoteAddress, sizeof(u_long));
	std::cout << inet_ntoa(Address) << ":" << nPort << std::endl;

	// Connect to the server
	std::cout << "Connecting to remote host..." << std::endl;
	SOCKET sd = EstablishConnection(nRemoteAddress, htons(nPort));
	if (sd == INVALID_SOCKET) { std::cout << WSAGetLastErrorMessage("connect to server") << std::endl; 	return 1; }
	std::cout << "Connected.  Socket " << sd << std::endl << std::endl;

	// set keyboard handler
	zSetKeyboardHandler();
	
	while (keep_running) {
		// Send the message to the server
		int n = rand() % 100 + 1;
		/*std::string strM = "My process ID = " + std::to_string(GetCurrentProcessId())
			+ " and my favorite number = " + std::to_string(n);*/
		std::string strM = std::to_string(GetCurrentProcessId()) + " " + std::to_string(n);
		const char* strMessage = strM.c_str();
		int nBytes;
		if (SendMessage(sd, strMessage)) {
			std::cout << "\tSuccessful send from client" << std::endl;

			nBytes = ReadReply(sd, strMessage);
			if (nBytes > 0) { 
				if (nBytes == kBufferSize) { 
					std::cout << "\tReceived length equals buffer size" << std::endl; } 
				else {
					// Receive is good
					std::cout << "\tSuccessful receive from server" << std::endl;
				}
			}
			else if (nBytes == 0) {
				std::cout << "\tServer unexpectedly closed the connection" << std::endl; }
			else {
				 std::cout << WSAGetLastErrorMessage("\tReceive message failed") << std::endl; return 1; }

		}
		else { 
			std::cout << "\tSend from client failed" << std::endl;
			std::cout << WSAGetLastErrorMessage("\tSend message failed") << std::endl; return 1; 
		}

		std::cout << std::endl;
		// this is the preferred method
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		//keep_running = false;
	}

	// Shut connection down
	std::cout << "Shutting connection down..." << std::flush;
	if (ShutdownConnection(sd)) { std::cout << "Connection is down." << std::endl; }
	else { std::cout << std::endl << WSAGetLastErrorMessage("Shutdown connection") << std::endl; }
	std::cout << "All done!" << std::endl;

	std::cout << "Normal exit, keep_running = " << keep_running << std::endl;
	std::cout << "Press Enter to exit" << std::endl;
	std::cin.get();
    // Shut Winsock down
    WSACleanup();
	return 0;
}



