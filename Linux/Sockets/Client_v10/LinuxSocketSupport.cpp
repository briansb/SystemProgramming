#include "SocketSupport.h"

// These are Windows-specific routines
#if defined(__linux__)

int client_socket;

std::string PrintBuffer(char *ptr, int n) {
	std::string s;
	for (int i = 0; i < n; ++i)
		s += *ptr++;
	return s;
}

bool InitializeSockets() {
	// Start Winsock - not needed for Linux
	return true;
}

bool CreateSocket() {
	// Windows

	// AF_INET = 2, The Internet Protocol version 4 (IPv4) address family.
	// SOCK_STREAM = 1, A socket type that provides sequenced, reliable, two-way, connection-based byte streams with an OOB (out of band) data transmission mechanism. This socket type uses the Transmission Control Protocol (TCP) for the Internet address family.
	

	// INVALID_SOCKET = ridiculous number, indicates error condition
	// If no error occurs, socket returns a descriptor referencing the new socket. 
	// Otherwise, a value of INVALID_SOCKET is returned, and a specific error code can be retrieved by calling WSAGetLastError.

	// Linux
	// socket() creates an endpoint for communication and returns a descriptor.
	// AF_INET = 2, The Internet Protocol version 4 (IPv4) address family.
	// SOCK_STREAM = 1, A socket type that provides sequenced, reliable, two-way, connection-based byte streams with an OOB (out of band) data transmission mechanism. This socket type uses the Transmission Control Protocol (TCP) for the Internet address family.
	// The protocol specifies a particular protocol to be used with the socket. Normally only a single protocol exists to support a particular socket type within a given protocol family, in which case protocol can be specified as 0
	// On success, a file descriptor for the new socket is returned. On error, -1 is returned, and errno is set appropriately.
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		std::cout << "Socket not created.  CreateSocket() returned error code " << client_socket << std::endl;
		return false;
	}
	return true;
}

bool ConnectToSocket() {
	// Windows
	// If no error occurs, connect returns zero. 
	// Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError.
	// Linux
	// The connect() system call connects the socket referred to by the file descriptor sockfd to the address specified by addr.  
	// The addrlen argument specifies the size of addr. 
	// If the connection or binding succeeds, zero is returned.  On error, -1 is returned, and errno is set appropriately.
	const char* host = "127.0.0.1";
	int intPort = 4242;
	u_long ulRemoteAddress = inet_addr(host);
	sockaddr_in sa_inServer;
	sa_inServer.sin_family = AF_INET;
	sa_inServer.sin_addr.s_addr = ulRemoteAddress;
	sa_inServer.sin_port = (u_short)htons(intPort);
	int result = connect(client_socket, (sockaddr*)&sa_inServer, sizeof(sockaddr_in));
	if (result < 0) {
		std::cout << "Could not connect to socket.  ConnectToSocket() returned error code " << result << std::endl;
		return false;
	}
	return true;
}

bool SendToServer(std::string s) {
	// Windows
	// Send the string to the server
	// If no error occurs, send returns the total number of bytes sent, 
	// which can be less than the number requested to be sent in the len parameter. 
	// Otherwise, a value of SOCKET_ERROR is returned, and a specific error code can be retrieved by calling WSAGetLastError
	// Linux
	// The system calls send(), sendto(), and sendmsg() are used to transmit a message to another socket.

	int bytes_sent = send(client_socket, s.c_str(), (int)s.length(), 0);
	if (bytes_sent < 0) { 
		std::cout << "Could not send to server.  SendToServer() returned error code " << bytes_sent << std::endl; 
		return false; 
	}
	return true;
}

bool ReceiveFromServer(std::string &strMessageFromServer) {
	// Read reply from server
	// If no error occurs, recv returns the number of bytes received, 
	// and the buffer pointed to by the buf parameter will contain this data received. 
	// If the connection has been gracefully closed, the return value is zero.
	// Otherwise, a value of SOCKET_ERROR is returned, and a specific error code can be retrieved by calling WSAGetLastError
	char buffer[1024];
	int bytes_received = recv(client_socket, buffer, 1024, 0);
	if (bytes_received < 0) {
		std::cout << "Receive from server error" << std::endl;
		return false;
	}
	else if (bytes_received == 0) { 
		std::cout << "Server unexpectedly closed connection" << std::endl; 
		return false;
	}
	else if (bytes_received < 0) { 
		std::cout << "Could not receive from server.  ReceiveFromServer() returned error code " << bytes_received << std::endl; 
		return false;
	}
	strMessageFromServer = PrintBuffer(buffer, bytes_received);
	return true;

}

bool SocketShutdown() {
	// // Shut connection down
	// std::cout << "Shutting connection down..." << std::endl;

	// // Disallow any further data sends.  Required for graceful exit.
	// // If no error occurs, shutdown returns zero. 
	// // Otherwise, a value of SOCKET_ERROR is returned, and a specific error code can be retrieved by calling WSAGetLastError.
	// int result = shutdown(client_socket, SD_SEND);
	// if (result == SOCKET_ERROR) std::cout << "SocketShutdown() shutdown() returned error code " << result << std::endl;

	// // Receive any extra data still sitting on the socket. Unlikely in current configuration. 
	// char buffer[1024];
	// result = recv(client_socket, buffer, 1024, 0);
	// if (result == SOCKET_ERROR) std::cout << "SocketShutdown() recv() returned error code " << result << " while clearing buffer" << std::endl;
	// else if (result != 0) std::cout << "Received " << result << " unexpected bytes during shutdown." << std::endl;

	// // Close the socket.
	// // If no error occurs, closesocket returns zero. 
	// // Otherwise, a value of SOCKET_ERROR is returned, and a specific error code can be retrieved by calling WSAGetLastError.
	// result = closesocket(client_socket);
	// if (result == SOCKET_ERROR) {
	// 	std::cout << "SocketShutdown() closesocket() returned error code " << result << "while closing socket" << std::endl;
	// 	return false;
	// }

	// // Shut Winsock down
	// // The return value is zero if the operation was successful. 
	// // Otherwise, the value SOCKET_ERROR is returned, and a specific error number can be retrieved by calling WSAGetLastError.
	// result = WSACleanup();
	// if (result != 0) { 
	// 	std::cout << "SocketShutdown WSACleanup() returned error code " << result << std::endl; 
	// 	return false; 
	// }

	return true;
}
#endif