#include "SocketSupport.h"

// These are Linux-specific routines
#if defined(__linux__)




// 
void zSocketInitializationAndStart();

// 
void zSocketShutdown();

// sends message to server
void zSendToServer(std::string strMessageToServer);

// receives return message from server, and checks for errors
void zReceiveFromServer(std::string strMessageToServer);
















#endif