#ifndef SOCKETSUPPORT_H
#define SOCKETSUPPORT_H


#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>					/* needed for a bunch of things */
	#define SD_SEND 1   /* needed for Winsock 1.1 */
#elif defined(__linux__)
	#include <unistd.h>
	#include<stdio.h>
	#include<string.h>	//strlen
	#include<stdlib.h>	//strlen
	#include <string>
	#include<sys/socket.h>
	#include<arpa/inet.h>	//inet_addr
	#include<unistd.h>	//write
	#include <pthread.h> //for threading , link with lpthread
#endif

#include <string>
#include <iostream>
#include <vector>


// writes a character buffer to a string
std::string BufferToString(char CharBuffer[], int buffer_size);

//parses incoming data
std::vector<std::string> Parse(std::string s, char delimiter);

bool InitializeSockets();
bool CreateSocket();
bool BindToSocket();
bool ListenOnSocket();

int AcceptConnection();
bool StartThread(int sock);
//DWORD WINAPI ReadAndWriteToClient(void* local_socket);
void *ReadAndWriteToClient(void* local_socket);

void ClientShutdown();

bool SocketShutdown();

#endif