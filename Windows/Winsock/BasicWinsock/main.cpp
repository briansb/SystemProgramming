#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#include <iostream>   //  I added this

#pragma comment(lib, "Ws2_32.lib")

// https://docs.microsoft.com/en-us/windows/win32/winsock/using-winsock

int main() {

	WSADATA wsaData;
	int iResult;
	// Initialize Winsock
	// The WSAStartup function is called to initiate use of WS2_32.dll.  The WSADATA structure contains information about the Windows Sockets implementation.
	// The MAKEWORD(2, 2) parameter of WSAStartup makes a request for version 2.2 of Winsock on the system, 
	// and sets the passed version as the highest version of Windows Sockets support that the caller can use.
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}


	printf("Normal exit\n");
	std::cin.get();

	return 0;
}