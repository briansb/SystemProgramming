// https://tangentsoft.net/wskfaq/
// excellent resource for both client and server

#include "WinsockServer.h"
#include "Support.h"

#pragma comment (lib, "Ws2_32.lib")

bool keep_running = true;


int main(int argc, char* argv[])
{
	// Start Winsock up
	WSAData wsaData;
	int nCode;
	if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) { std::cout << "WSAStartup() returned error code " << nCode << "." << std::endl; return 255; }

	const char* pcHost = "0.0.0.0";
	int nPort = 4242;
	
	std::cout << "Establishing the listener..." << std::endl;
	SOCKET ListeningSocket = SetUpListener(pcHost, htons(nPort));
	if (ListeningSocket == INVALID_SOCKET) {
		std::cout << std::endl << WSAGetLastErrorMessage("establish listener") << std::endl;
		return 3;
	}
	
	std::cout << "Waiting for connections..." << std::flush;
	/***** AcceptConnections(ListeningSocket); *********/
	// AcceptConnections - Spins forever waiting for connections.  For each one that comes in, 
	// we create a thread to handle it and go back to waiting for connections.  If an error occurs, we return.
	sockaddr_in sinRemote;
	int nAddrSize = sizeof(sinRemote);

	// set keyboard handler
	zSetKeyboardHandler();

	while (keep_running) {
		printf("keep_running = %d\n", keep_running);
		SOCKET sd = accept(ListeningSocket, (sockaddr*)&sinRemote, &nAddrSize);
		if (sd != INVALID_SOCKET) {
			std::cout << "Accepted connection from " << 	inet_ntoa(sinRemote.sin_addr) << ":" <<
				ntohs(sinRemote.sin_port) << "." <<
				std::endl;

			DWORD nThreadID;
			CreateThread(0, 0, EchoHandler, (void*)sd, 0, &nThreadID);
		}
		else {
			std::cout << WSAGetLastErrorMessage("accept() failed") <<
				std::endl;
			return 1;
		}
	}
	

	std::cout << "Normal exit, keep_running = " << keep_running << std::endl;
	std::cout << "Press Enter to exit" << std::endl;
	std::cin.get();
	// Shut Winsock down
	WSACleanup();
	return 0;
}


