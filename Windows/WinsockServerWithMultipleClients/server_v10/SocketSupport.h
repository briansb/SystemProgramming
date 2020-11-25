#ifndef SOCKETSUPPORT_H
#define SOCKETSUPPORT_H


#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>					/* needed for a bunch of things */
	#define SD_SEND 1   /* needed for Winsock 1.1 */
#elif defined(__linux__)
	#include <unistd.h>
#endif

#include <winsock.h>
#include <iostream>
#include <strstream>
#include <algorithm>
#include <string>
#include <vector>

// writes a character buffer to a string
std::string BufferToString(char CharBuffer[], int buffer_size);

//parses incoming data
std::vector<std::string> Parse(std::string s, char delimiter);

bool InitializeSockets();
bool CreateSocket();
bool BindToSocket();
bool ListenOnSocket();

SOCKET AcceptConnection();
bool StartThread(SOCKET sock);
DWORD WINAPI ReadAndWriteToClient(void* local_socket);

void ClientShutdown(SOCKET sd);

bool SocketShutdown();





#endif