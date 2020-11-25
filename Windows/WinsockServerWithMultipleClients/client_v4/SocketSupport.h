#ifndef SOCKETSUPPORT_H
#define SOCKETSUPPORT_H


#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>					/* needed for a bunch of things */
#elif defined(__linux__)
	#include <unistd.h>
#endif

#include <iostream>
#include <string>
#include <strstream>
#include <algorithm>

#define SD_SEND 1   /* needed for Winsock 1.1 */

// kBufferSize must be larger than the length of the message
const int kBufferSize = 1024;

// OS-dependent routines
// Starts Winsock, finds the server, and creates a socket
void zSocketInitializationAndStart();

// clears the buffer of unread data, and closes the socket
void zSocketShutdown();

// sends message to server
void zSendToServer(std::string strMessageToServer);

// receives return message from server, and checks for errors
void zReceiveFromServer(std::string strMessageToServer);



#endif