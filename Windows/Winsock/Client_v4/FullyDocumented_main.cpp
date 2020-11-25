#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
// #include <iphlpapi.h>  // might need this
#include <stdlib.h>
#include <stdio.h>

#include <iostream>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
// or link in IDE
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"


// int main(int argc, char** argv) {
int __cdecl fully_documented_main(int argc, char **argv)
{
	WSADATA wsaData;

	// Create a SOCKET object called ConnectSocket.
	SOCKET ConnectSocket = INVALID_SOCKET;

	// Declare an addrinfo object that contains a sockaddr structure and initialize these values. 
	// For this application, the Internet address family is unspecified so that either an IPv6 or IPv4 address can be returned. 
	// The application requests the socket type to be a stream socket for the TCP protocol.
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	const char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Validate the parameters
	if (argc != 2) {
		printf("usage: %s server-name\n", argv[0]);
		return 1;
	}

	// Initialize Winsock
	// The WSAStartup function is called to initiate use of WS2_32.dll.  The WSADATA structure contains information about the Windows Sockets implementation.
	// The MAKEWORD(2, 2) parameter of WSAStartup makes a request for version 2.2 of Winsock on the system, 
	// and sets the passed version as the highest version of Windows Sockets support that the caller can use.
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Call the getaddrinfo function requesting the IP address for the server name passed on the command line. 
	// The TCP port on the server that the client will connect to is defined by DEFAULT_PORT as 27015 in this sample. 
	// The getaddrinfo function returns its value as an integer that is checked for errors.
	// Resolve the server address and port
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Call the socket function and return its value to the ConnectSocket variable. 
	// For this application, use the first IP address returned by the call to getaddrinfo that matched the address family, 
	// socket type, and protocol specified in the hints parameter. In this example, a TCP stream socket was specified with a socket type 
	// of SOCK_STREAM and a protocol of IPPROTO_TCP. The address family was left unspecified (AF_UNSPEC), 
	// so the returned IP address could be either an IPv6 or IPv4 address for the server.
	// If the client application wants to connect using only IPv6 or IPv4, then the address family needs to be set to AF_INET6 for IPv6 or AF_INET for IPv4 in the hints parameter.
	// Attempt to connect to the first address returned by the call to getaddrinfo
	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		// Create a SOCKET for connecting to server
		// The parameters passed to the socket function can be changed for different implementations.
		// Error detection is a key part of successful networking code.  If the socket call fails, it returns INVALID_SOCKET.
		// The if statement in the previous code is used to catch any errors that may have occurred while creating the socket.
		// WSAGetLastError returns an error number associated with the last error that occurred.
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) { printf("socket failed with error: %ld\n", WSAGetLastError()); WSACleanup(); return 1; }

		// Connect to server.
		// Call the connect function, passing the created socket and the sockaddr structure as parameters. Check for general errors.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) { closesocket(ConnectSocket); ConnectSocket = INVALID_SOCKET; continue; }
		break;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message
	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) { printf("Unable to connect to server!\n"); WSACleanup(); return 1; }

	// Send an initial buffer
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) { printf("send failed with error: %d\n", WSAGetLastError()); closesocket(ConnectSocket); WSACleanup(); return 1; }
	printf("Bytes Sent: %ld\n", iResult);

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) { printf("shutdown failed with error: %d\n", WSAGetLastError()); closesocket(ConnectSocket); WSACleanup(); return 1; }

	// Receive until the peer closes the connection
	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Bytes received: %d\n", iResult);
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	printf("Normal exit\n");
	std::cin.get();

	return 0;
}
