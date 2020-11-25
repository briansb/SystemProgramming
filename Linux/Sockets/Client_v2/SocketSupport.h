#ifndef SOCKETSUPPORT_H
#define SOCKETSUPPORT_H

#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>					/* needed for a bunch of things */
#elif defined(__linux__)
	#include <unistd.h>
#endif

#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdio.h>	//printf
#include <string.h>	//strlen
#include <arpa/inet.h>	//inet_addr
// OS-dependent routines
// 
void zSocketInitializationAndStart();

// 
void zSocketShutdown();

// sends message to server
void zSendToServer(std::string strMessageToServer);

// receives return message from server, and checks for errors
void zReceiveFromServer(std::string strMessageToServer);






#endif