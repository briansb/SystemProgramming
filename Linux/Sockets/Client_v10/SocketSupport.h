#ifndef SOCKETSUPPORT_H
#define SOCKETSUPPORT_H


#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>   /* needed for a bunch of things */
	#define SD_SEND 1   /* needed for Winsock 1.1 */
	#pragma comment (lib, "Ws2_32.lib")
#elif defined(__linux__)
	#include <unistd.h>   /* needed for a bunch of things */
	#include <string.h>	//strlen
	#include <sys/socket.h>	//socket
	#include <arpa/inet.h>	//inet_addr
#endif

#include <iostream>
#include <string>

bool InitializeSockets();
bool CreateSocket();
bool ConnectToSocket();
bool SendToServer(std::string strMessageToServer);
bool ReceiveFromServer(std::string &strMessageFromServer);
bool SocketShutdown();

#endif