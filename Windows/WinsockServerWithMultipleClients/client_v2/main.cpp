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
    if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) { std::cout << "WSAStartup() returned error code " << nCode << "." << 	std::endl; return 255; }

	const char* pcHost = "127.0.0.1";
	int nPort = 4242;
	// Find the server's address
	std::cout << "Looking up address..." << std::flush;
	u_long nRemoteAddress = LookupAddress(pcHost);
	if (nRemoteAddress == INADDR_NONE) { std::cout << std::endl << WSAGetLastErrorMessage("lookup address") << std::endl; return 3; }
	in_addr Address;
	memcpy(&Address, &nRemoteAddress, sizeof(u_long));
	std::cout << inet_ntoa(Address) << ":" << nPort << std::endl;

	// Connect to the server
	std::cout << "Connecting to remote host..." << std::flush;
	SOCKET sd = EstablishConnection(nRemoteAddress, htons(nPort));
	if (sd == INVALID_SOCKET) { std::cout << std::endl << WSAGetLastErrorMessage("connect to server") << std::endl; 	return 3; }
	std::cout << "Connected.  Socket " << sd << "." << std::endl;

	// set keyboard handler
	zSetKeyboardHandler();

	while (keep_running) {
		// Send the echo packet to the server
		const char* strMessage = "This is a test of the emergency data transfer system.";
		const size_t strMessageLen = strlen(strMessage);
		std::cout << "Sending echo packet (" << strMessageLen << " bytes)..." << std::flush;
		int nBytes;
		if (SendEcho(sd)) {
			std::cout << std::endl;
			if ((nBytes = ReadReply(sd)) > 0) { if (nBytes == kBufferSize) { std::cout << "FYI, likely data overflow." << std::endl; } }
			else if (nBytes == 0) { std::cout << std::endl << "Server unexpectedly closed the connection" << std::endl; }
			else { std::cout << std::endl << WSAGetLastErrorMessage("read reply") << std::endl; 	return 3; }
		}
		else { std::cout << std::endl << WSAGetLastErrorMessage("send echo packet") << std::endl; return 3; }

		// this is the preferred method
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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



