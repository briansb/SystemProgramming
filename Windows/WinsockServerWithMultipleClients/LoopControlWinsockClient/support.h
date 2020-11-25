#pragma once

#include <Windows.h>
//#include <time.h>
#include <string>
#include <algorithm>
#include <winsock.h>
//#include <stdlib.h>

extern const char* WSAGetLastErrorMessage(const char* pcMessagePrefix, int nErrorID = 0);
extern bool ShutdownConnection(SOCKET sd);
void InitializeWinsockAndConnectToServer(SOCKET &sd);
u_long LookupAddress(const char* pcHost);
SOCKET EstablishConnection(u_long nRemoteAddr, u_short nPort);
void CloseConnectionAndShutdownWinsock(SOCKET &sd);