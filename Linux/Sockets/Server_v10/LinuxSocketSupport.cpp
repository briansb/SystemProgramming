#include "SocketSupport.h"

std::string BufferToString(char CharBuffer[], int buffer_size) {
	int i = 0;
	std::string s;
	while (i < buffer_size) { s += CharBuffer[i++]; }
	return s;
}

std::vector<std::string> Parse(std::string s, char delimiter) {
	std::vector<std::string> data;
	std::string buffer;
	for (size_t i = 0; i < s.length(); ++i) {
		if (s[i] == delimiter) {
			data.push_back(buffer);
			buffer = "";
		}
		else {
			buffer += s[i];
		}
	}
	data.push_back(buffer);

	return data;
}

// These are Linux-specific routines
#if defined(__linux__)

int client_socket;
int ListeningSocket;
sockaddr_in sinRemote;
int nAddrSize;

bool InitializeSockets() {
	// Start Winsock - not needed for Linux
	return true;
}

bool CreateSocket() {
	// The socket function creates a socket ( a communication endpoint) that is bound to a specific transport service provider
	// Create a stream socket
	// AF_INET = 2, The Internet Protocol version 4 (IPv4) address family.
	// SOCK_STREAM = 1, A socket type that provides sequenced, reliable, two-way, connection-based byte streams with an OOB (out of band) data transmission mechanism. This socket type uses the Transmission Control Protocol (TCP) for the Internet address family.
	// INVALID_SOCKET = ridiculous number, indicates error condition
	// If no error occurs, socket returns a descriptor referencing the new socket. 
	// Otherwise, a value of INVALID_SOCKET is returned, and a specific error code can be retrieved by calling WSAGetLastError.
	ListeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ListeningSocket < 0) {
		std::cout << "Socket not created.  CreateSocket() returned error code " << ListeningSocket << std::endl;
		return false;
	}
	return true;
}

bool BindToSocket() {
	// The bind function associates a local address with a socket
	// If no error occurs, bind returns zero. 
	// Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError.
	int result;
	const char* pcHost = "0.0.0.0";
	int nPort = 4242;
	u_long nInterfaceAddr = inet_addr(pcHost);
	sockaddr_in sinInterface;
	sinInterface.sin_family = AF_INET;
	sinInterface.sin_addr.s_addr = nInterfaceAddr;
	sinInterface.sin_port = htons(nPort);
	result = bind(ListeningSocket, (sockaddr*)&sinInterface, sizeof(sockaddr_in));
	if (result < 0) {
		std::cout << "Could not bind to socket.  BindToSocket() returned error code " << result << std::endl;
		return false;
	}
	return true;
}

bool ListenOnSocket() {
	// The listen function places a socket in a state in which it is listening for an incoming connection
	// If no error occurs, listen returns zero. 
	// Otherwise, a value of SOCKET_ERROR is returned, and a specific error code can be retrieved by calling WSAGetLastError.
	int result = listen(ListeningSocket, SOMAXCONN);
	if (result < 0) {
		std::cout << "Can not listen on socket. ListenOnSocket() returned error code " << result << std::endl;
		return false;
	}
	return true;

}

int AcceptConnection() {
	// The accept function permits an incoming connection attempt on a socket
	// If no error occurs, accept returns a value of type SOCKET that is a descriptor for the new socket. 
	// This returned value is a handle for the socket on which the actual connection is made.
	// Otherwise, a value of INVALID_SOCKET is returned, and a specific error code can be retrieved by calling WSAGetLastError.
	nAddrSize = sizeof(sinRemote);
	int sock = accept(ListeningSocket, (sockaddr*)&sinRemote, (socklen_t*)&nAddrSize);
	if (sock < 0) {
		std::cout << "Accept failed from " << inet_ntoa(sinRemote.sin_addr) << ":" << ntohs(sinRemote.sin_port) << std::endl;
	}
	else {
		std::cout << "Accepted connection from " << inet_ntoa(sinRemote.sin_addr) << ":" << ntohs(sinRemote.sin_port) << std::endl;
	}
	return sock;
}

bool StartThread(int sock) {
	//long nThreadID;
	int *new_sock;
	//CreateThread(0, 0, ReadAndWriteToClient, (void*)sock, 0, &nThreadID);
	pthread_t sniffer_thread;
	new_sock = (int*)malloc(1);
	*new_sock = sock;
		
	pthread_create( &sniffer_thread , NULL ,  ReadAndWriteToClient , (void*) new_sock);
	//std::cout << "Started thread " << nThreadID << std::endl;
	return true;
}

void *ReadAndWriteToClient(void* local_socket)
{
	//int sd = *(int)*local_socket;
	int sock = *(int*)local_socket;
	int bytes_received;
	int bytes_sent;
	std::string strMessageFromClient;
	std::string strMessageToClient;
	char buffer[1024];
	std::vector<std::string> incoming_data;
	
	while (1)
	{
		bytes_received = recv(sock, buffer, 1024, 0);
		if (bytes_received > 0) {
			// good receive
			strMessageFromClient = BufferToString(buffer, bytes_received);
			std::cout << "Received from client:  " << strMessageFromClient << std::endl;
			// send something back
			incoming_data = Parse(strMessageFromClient, ' ');
			strMessageToClient = incoming_data[3] + " plus " + incoming_data[7];

			bytes_sent = send(sock, strMessageToClient.c_str(), (int)strMessageToClient.length(), 0);
			if (bytes_sent < 0) {
				std::cout << "ReadAndWriteToClient send() reports SOCKET_ERROR" << std::endl;
				break;
			}
			else {
				std::cout << "Sent to client:  " << strMessageToClient << std::endl;
			}
		}
		else if (bytes_received == 0) {
			// socket has been closed
			std::cout << "ReadAndWriteToClient recv() reports client socket " << sock << " closed" << std::endl;
			break;
		}
		else if (bytes_received < 0) {
			// something bad happened
			std::cout << "ReadAndWriteToClient recv() reports SOCKET_ERROR" << std::endl;
			break;
		}
		else {
			// who knows what happened
			std::cout << "ReadAndWriteToClient recv() reports unknown error" << std::endl;
			break;
		}
	} 

	ClientShutdown();

	return 0;
}

void ClientShutdown()
{
	// // Shuts down the socket, checks for unexpected send, and closes the socket
	// // Disallow any further data sends.  This will tell the other side
	// // that we want to go away now.  If we skip this step, we don't
	// // shut the connection down nicely.
	// if (shutdown(sd, SD_SEND) < 0) { std::cout << "Could not shut down socket.  ClientShutdown() returned SOCKET_ERROR" << std::endl; }

	// // Receive any extra data still sitting on the socket.  After all
	// // data is received, this call will block until the remote host
	// // acknowledges the TCP control packet sent by the shutdown above.
	// // Then recv will return 0, signaling that the remote host has closed its side of the connection.
	// char buffer[1024];
	// int last_bytes = recv(sd, buffer, 1024, 0);
	// if (last_bytes == SOCKET_ERROR) { std::cout << "SOCKET_ERROR on last receive" << std::endl; }
	// else if (last_bytes != 0) { std::cout << "Received unexpected bytes during shutdown." << std::endl; }

	// // Close the socket.
	// if (closesocket(sd) == SOCKET_ERROR) { std::cout << "Could not close socket.  ClientShutdown() returned SOCKET_ERROR" << std::endl; }
}

bool SocketShutdown() {
	// shut down server socket
	int result = shutdown(ListeningSocket, SHUT_RDWR);
	if (result != 0) {
		std::cout << "Could not shut down listening socket.  Returned error code " << result << std::endl;
		return false;
	}
	return true;
}
#endif