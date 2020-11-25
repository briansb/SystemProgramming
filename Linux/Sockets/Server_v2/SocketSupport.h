#ifndef SOCKETSUPPORT_H
#define SOCKETSUPPORT_H

#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>					/* needed for a bunch of things */
#elif defined(__linux__)
	#include <unistd.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <sys/types.h> 
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif

 
#include <string>



// OS-dependent routines
// Create socket and start listener
void zSocketInitialization();

// Close socket
void zSocketShutdown();

// Accepts connection from client, then forks a new process
void zAcceptConnectionAndStartThread();

// There is a separate instance of this function for each connection.  
// It handles all communicationonce a connnection has been established.
void ReceiveAndSendToClient(int sock);

// error messages
void error(const char *msg);




#endif
