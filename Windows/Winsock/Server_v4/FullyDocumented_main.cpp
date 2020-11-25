#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
// #include <iphlpapi.h>   // might need this
#include <stdlib.h>
#include <stdio.h>

#include <iostream>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"


int __cdecl fully_documented_main(void)
{
	WSADATA wsaData;
	int iResult;

	// Create a SOCKET object called ListenSocket for the server to listen for client connections.
	SOCKET ListenSocket = INVALID_SOCKET;
	// Create a temporary SOCKET object called ClientSocket for accepting connections from clients.
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// To receive and send data on a socket
	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	// The WSAStartup function is called to initiate use of WS2_32.dll.  The WSADATA structure contains information about the Windows Sockets implementation.
	// The MAKEWORD(2, 2) parameter of WSAStartup makes a request for version 2.2 of Winsock on the system, 
	// and sets the passed version as the highest version of Windows Sockets support that the caller can use.
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	// The getaddrinfo function is used to determine the values in the sockaddr structure:
	// AF_INET is used to specify the IPv4 address family.
	// SOCK_STREAM is used to specify a stream socket.
	// IPPROTO_TCP is used to specify the TCP protocol .
	// AI_PASSIVE flag indicates the caller intends to use the returned socket address structure in a call to the bind function.
	// When the AI_PASSIVE flag is set and nodename parameter to the getaddrinfo function is a NULL pointer, 
	// the IP address portion of the socket address structure is set to INADDR_ANY for IPv4 addresses or IN6ADDR_ANY_INIT for IPv6 addresses.
	// 27015 is the port number associated with the server that the client will connect to.
	// The addrinfo structure is used by the getaddrinfo function.
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Call the socket function and return its value to the ListenSocket variable. 
	// For this server application, use the first IP address returned by the call to getaddrinfo that matched the address family, socket type, and protocol specified in the hints parameter. 
	// In this example, a TCP stream socket for IPv4 was requested with an address family of IPv4, a socket type of SOCK_STREAM and a protocol of IPPROTO_TCP. 
	// So an IPv4 address is requested for the ListenSocket.
	// If the server application wants to listen on IPv6, then the address family needs to be set to AF_INET6 in the hints parameter.
	// If a server wants to listen on both IPv6 and IPv4, two listen sockets must be created, one for IPv6 and one for IPv4.
	// These two sockets must be handled separately by the application.
	// Windows Vista and later offer the ability to create a single IPv6 socket that is put in dual stack mode to listen on both IPv6 and IPv4.
	// For more information on this feature, see Dual - Stack Sockets.

	// Create a SOCKET for the server to listen for client connections
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) { printf("socket failed with error: %ld\n", WSAGetLastError()); freeaddrinfo(result); WSACleanup(); return 1; }

	// The sockaddr structure holds information regarding the address family, IP address, and port number.
	// Call the bind function, passing the created socket and sockaddr structure returned from the getaddrinfo function as parameters.Check for general errors.
	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) { printf("bind failed with error: %d\n", WSAGetLastError()); freeaddrinfo(result); closesocket(ListenSocket); WSACleanup(); return 1; }

	// Once the bind function is called, the address information returned by the getaddrinfo function is no longer needed. 
	// The freeaddrinfo function is called to free the memory allocated by the getaddrinfo function for this address information.
	freeaddrinfo(result);

	// Call the listen function, passing as parameters the created socket and a value for the backlog, maximum length of the queue of pending connections to accept. 
	// In this example, the backlog parameter was set to SOMAXCONN. 
	// This value is a special constant that instructs the Winsock provider for this socket to allow a maximum reasonable number of pending connections in the queue. 
	// Check the return value for general errors.
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) { printf("listen failed with error: %d\n", WSAGetLastError()); closesocket(ListenSocket); WSACleanup(); return 1; }

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) { printf("accept failed with error: %d\n", WSAGetLastError()); closesocket(ListenSocket); WSACleanup(); return 1; }

	// No longer need server socket
	closesocket(ListenSocket);
	//  When the client connection has been accepted, a server application would normally pass the accepted client socket(the ClientSocket variable in the above sample code) 
	//  to a worker thread or an I / O completion port and continue accepting additional connections.
	//  In this basic example, the server continues to the next step.
	//  There are a number of other programming techniques that can be used to listen for and accept multiple connections.
	//  These include using the select or WSAPoll functions.  
	//  Examples of some of these various programming techniques are illustrated in the Advanced Winsock Samples included with the Microsoft Windows Software Development Kit(SDK).

	// Receive until the peer shuts down the connection
	do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) { printf("send failed with error: %d\n", WSAGetLastError()); closesocket(ClientSocket); WSACleanup(); return 1; }
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)  // caused by Client executing shutdown(ConnectSocket,...
			printf("Connection closing...\n");
		else { printf("recv failed with error: %d\n", WSAGetLastError()); closesocket(ClientSocket); WSACleanup(); return 1; }

	} while (iResult > 0);
	// The send and recv functions both return an integer value of the number of bytes sent or received, respectively, or an error.
	// Each function also takes the same parameters : the active socket, a char buffer, the number of bytes to send or receive, and any flags to use.

	// To disconnect and shutdown a socket
	// When the server is done sending data to the client, the shutdown function can be called specifying SD_SEND to shutdown the sending side of the socket.
	// This allows the client to release some of the resources for this socket.The server application can still receive data on the socket.
	// Shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) { printf("shutdown failed with error: %d\n", WSAGetLastError()); closesocket(ClientSocket); WSACleanup(); return 1; }

	// When the client application is done receiving data, the closesocket function is called to close the socket.
	// When the client application is completed using the Windows Sockets DLL, the WSACleanup function is called to release resources.

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	printf("Normal exit\n");
	std::cin.get();
	return 0;
}
