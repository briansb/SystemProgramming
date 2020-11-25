#include "support.h"

#include <winsock.h>

#include <iostream>

// kBufferSize must be larger than the length of kpcEchoMessage.
const int kBufferSize = 1024;
const char* kpcEchoMessage = "This is a test of the emergency data transfer system.  If this had been real a real emergency, we would have sent this data out-of-band.";
const int kEchoMessageLen = strlen(kpcEchoMessage);

void InitializeWinsockAndConnectToServer(SOCKET &sd);
bool SendEcho(SOCKET sd);
int ReadReply(SOCKET sd);
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);
bool keep_running = true;
SOCKET sd;

int main(int argc, char* argv[])
{
	InitializeWinsockAndConnectToServer(sd);
	// Call the main example routine.
	// DoWinsock("127.0.0.1", 4242);

	int nBytes;
	if (SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
		while (keep_running) {
			// Send the echo packet to the server
			if (SendEcho(sd)) {
				if ((nBytes = ReadReply(sd)) > 0) {
					if (nBytes == kBufferSize) { std::cout << "FYI, likely data overflow." << std::endl; }
				}
				else if (nBytes == 0) { std::cerr << std::endl << "Server unexpectedly closed the connection" << std::endl; }
				else { std::cout << WSAGetLastErrorMessage("read reply") << std::endl; return 3; }
			}
			else { std::cout << WSAGetLastErrorMessage("send echo packet") << std::endl; return 3; }
			Sleep(1000);
		}
	}


	CloseConnectionAndShutdownWinsock(sd);
	
	std::cout << "Press Enter to exit" << std::endl;
	std::cin.get();
	return 0;

}
bool SendEcho(SOCKET sd) {
	// Send the string to the server
	std::cout << "Sending echo packet (" << strlen(kpcEchoMessage) << " bytes)..." << std::endl;
	if (send(sd, kpcEchoMessage, kEchoMessageLen, 0) != SOCKET_ERROR) { return true; }
	else { return false; }
}

// Read the reply packet and check it for sanity.  Returns -1 on error, 0 on connection closed, > 0 on success.
int ReadReply(SOCKET sd) {
	// Read reply from server
	char acReadBuffer[kBufferSize];
	int nTotalBytes = 0;
	while (nTotalBytes < kEchoMessageLen) {
		int nNewBytes = recv(sd, acReadBuffer + nTotalBytes,
			kBufferSize - nTotalBytes, 0);
		if (nNewBytes == SOCKET_ERROR) { return -1; }
		else if (nNewBytes == 0) { std::cerr << "Connection closed by peer." << std::endl; return 0; }
		nTotalBytes += nNewBytes;
	}

	// Check data for sanity
	if (strncmp(acReadBuffer, kpcEchoMessage, nTotalBytes) == 0) { std::cout << "Reply packet matches" << std::endl; }
	else { std::cerr << "Mismatch in data received from server. " << "Something's broken!" << std::endl; }
	return nTotalBytes;
}


BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
	switch (fdwCtrlType) {
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		printf("Ctrl-C event\n\n");
		Beep(750, 300);
		keep_running = false;
		return TRUE;
		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:
		Beep(900, 200);
		printf("Ctrl-Break event\n\n");
		return FALSE;
	default:
		return FALSE;
	}
}