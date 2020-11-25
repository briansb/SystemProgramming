#include "SocketSupport.h"

// These are Windows-specific routines
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>					/* needed for a bunch of things */



#pragma comment (lib, "Ws2_32.lib")

SOCKET ListeningSocket;
sockaddr_in sinRemote;
int nAddrSize;

void zSocketInitialization() {
	// Start Winsock up
	WSAData wsaData;
	int nCode;
	if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) { std::cout << "WSAStartup() returned error code " << nCode << "." << std::endl; return; }

	const char* pcHost = "0.0.0.0";
	int nPort = 4242;

	std::cout << "Establishing the listener..." << std::endl;
	ListeningSocket = SetUpListener(pcHost, htons(nPort));
	if (ListeningSocket == INVALID_SOCKET) {
		std::cout << std::endl << WSAGetLastErrorMessage("establish listener") << std::endl;
		return;
	}

	std::cout << "Waiting for connections..." << std::endl;
	
	nAddrSize = sizeof(sinRemote);

}

void zAcceptConnectionAndStartThread(int stop) {
	std::cout << "Before accept" << std::endl;
	SOCKET sd = accept(ListeningSocket, (sockaddr*)&sinRemote, &nAddrSize);
	std::cout << "After accept" << std::endl;
	if (sd != INVALID_SOCKET) {
		std::cout << "Accepted connection from " << inet_ntoa(sinRemote.sin_addr) << ":" <<
			ntohs(sinRemote.sin_port) << "." <<
			std::endl;

		DWORD nThreadID;
		std::cout << "Before thread" << std::endl;
		CreateThread(0, 0, EchoHandler, (void*)sd, 0, &nThreadID);
		std::cout << "After thread" << std::endl;
	}
	else {
		std::cout << WSAGetLastErrorMessage("accept() failed") <<
			std::endl;
		return;
	}

}


void zSocketShutdown() {
	// Shut Winsock down
	WSACleanup();
}

std::string BufferToString(char CharBuffer[], int buffer_size) {
	int i = 0;
	std::string s;
	while (i < buffer_size) {
		s += CharBuffer[i++];
	}
	return s;
}


SOCKET SetUpListener(const char* pcAddress, int nPort)
{
	u_long nInterfaceAddr = inet_addr(pcAddress);
	if (nInterfaceAddr != INADDR_NONE) {
		SOCKET sd = socket(AF_INET, SOCK_STREAM, 0);
		if (sd != INVALID_SOCKET) {
			sockaddr_in sinInterface;
			sinInterface.sin_family = AF_INET;
			sinInterface.sin_addr.s_addr = nInterfaceAddr;
			sinInterface.sin_port = nPort;
			if (bind(sd, (sockaddr*)&sinInterface,
				sizeof(sockaddr_in)) != SOCKET_ERROR) {
				listen(sd, SOMAXCONN);
				return sd;
			}
			else {
				std::cout << WSAGetLastErrorMessage("bind() failed") <<
					std::endl;
			}
		}
	}

	return INVALID_SOCKET;
}



DWORD WINAPI EchoHandler(void* sd_)
{
	// this is the thread
	// it stays in EchoIncomingPackets as long as the client is talking
	std::cout << "At start of EchoHandler" << std::endl;
	int nRetval = 0;
	SOCKET sd = (SOCKET)sd_;

	if (!EchoIncomingPackets(sd)) {
		std::cout << std::endl << WSAGetLastErrorMessage(
			"Echo incoming packets failed") << std::endl;
		nRetval = 3;
	}

	std::cout << "Shutting connection down..." << std::flush;
	if (ShutdownConnection(sd)) {
		std::cout << "Connection is down." << std::endl;
	}
	else {
		std::cout << std::endl << WSAGetLastErrorMessage(
			"Connection shutdown failed") << std::endl;
		nRetval = 3;
	}

	std::cout << "At end of EchoHandler" << std::endl;
	return nRetval;
}


bool EchoIncomingPackets(SOCKET sd)
{
	// this is part of the thread above...it just loops and loops
	std::cout << "At start of EchoIncomingPackets" << std::endl;
	// Read data from client
	char acReadBuffer[kBufferSize];
	int nReadBytes;
	do {
		nReadBytes = recv(sd, acReadBuffer, kBufferSize, 0);
		if (nReadBytes > 0) {
			//std::cout << "Received " << nReadBytes << " bytes from client." << std::endl;
			//std::cout << "Received message:  " << BufferToString(acReadBuffer, nReadBytes) << std::endl;
			// parse string
			std::string s = BufferToString(acReadBuffer, nReadBytes);
			//std::cout << "Parsing:  " << s << std::endl;
			std::string delimiter = " ";
			size_t pos = 0;
			std::string token;
			std::string strMessage;
			while ((pos = s.find(delimiter)) != std::string::npos) {
				token = s.substr(0, pos);
				//std::cout << token << std::endl;
				s.erase(0, pos + delimiter.length());
			}
			//std::cout << s << std::endl;
			std::cout << "Client PID " << token << " sent data value " << s << std::endl;
			//std::cout << "Length:  " << nReadBytes << " bytes" << std::endl;
			//std::cout << "\tSuccessful receive from client" << std::endl;

			int nSentBytes = 0;
			while (nSentBytes < nReadBytes) {
				//int nTemp = send(sd, acReadBuffer + nSentBytes, nReadBytes - nSentBytes, 0);
				strMessage = s + " " + inet_ntoa(sinRemote.sin_addr) + ":" + std::to_string(ntohs(sinRemote.sin_port)) + " " + token;
				int nTemp = send(sd, strMessage.c_str(), (int)strMessage.length(), 0);
				std::cout << "Sent data value " << s << " from server connection " << inet_ntoa(sinRemote.sin_addr) <<
					":" << std::to_string(ntohs(sinRemote.sin_port)) << " to client PID " << token << std::endl << std::endl;
				if (nTemp > 0) {
					//printf("Sent to client:  %s\n", strMessage.c_str());
					//std::cout << "Sent to message:  " << BufferToString(acReadBuffer + nSentBytes, nReadBytes - nSentBytes) << std::endl;
					//std::cout << "Length:  " << nReadBytes - nSentBytes << " bytes" << std::endl;
					//std::cout << "\tSuccessful send from server" << std::endl;
					//std::cout << std::endl;
					nSentBytes += nTemp;
				}
				else if (nTemp == SOCKET_ERROR) { return false; }
				else {
					// Client closed connection before we could reply to
					// all the data it sent, so bomb out early.
					std::cout << "Peer unexpectedly dropped connection!" << std::endl;
					return true;
				}
			}
		}
		else if (nReadBytes == SOCKET_ERROR) {
			return false;
		}
	} while (nReadBytes != 0);

	std::cout << "At end of EchoIncomingPackets" << std::endl;
	std::cout << "Connection closed by peer." << std::endl;
	return true;
}


bool ShutdownConnection(SOCKET sd)
{
	// Disallow any further data sends.  This will tell the other side
	// that we want to go away now.  If we skip this step, we don't
	// shut the connection down nicely.
	if (shutdown(sd, SD_SEND) == SOCKET_ERROR) {
		return false;
	}

	// Receive any extra data still sitting on the socket.  After all
	// data is received, this call will block until the remote host
	// acknowledges the TCP control packet sent by the shutdown above.
	// Then we'll get a 0 back from recv, signalling that the remote
	// host has closed its side of the connection.
	char acReadBuffer[kBufferSize];
	while (1) {
		int nNewBytes = recv(sd, acReadBuffer, kBufferSize, 0);
		if (nNewBytes == SOCKET_ERROR) {
			return false;
		}
		else if (nNewBytes != 0) {
			std::cout << std::endl << "FYI, received " << nNewBytes <<
				" unexpected bytes during shutdown." << std::endl;
		}
		else {
			// Okay, we're done!
			break;
		}
	}

	// Close the socket.
	if (closesocket(sd) == SOCKET_ERROR) {
		return false;
	}

	return true;
}


const char* WSAGetLastErrorMessage(const char* pcMessagePrefix, int nErrorID)
{
	// Build basic error string
	static char acErrorBuffer[256];
	std::ostrstream outs(acErrorBuffer, sizeof(acErrorBuffer));
	outs << pcMessagePrefix << ": ";

	// Tack appropriate canned message onto end of supplied message 
	// prefix. Note that we do a binary search here: gaErrorList must be
	// sorted by the error constant's value.
	ErrorEntry* pEnd = gaErrorList + kNumMessages;
	ErrorEntry Target(nErrorID ? nErrorID : WSAGetLastError());
	ErrorEntry* it = std::lower_bound(gaErrorList, pEnd, Target);
	if ((it != pEnd) && (it->nID == Target.nID)) {
		outs << it->pcMessage;
	}
	else {
		// Didn't find error in list, so make up a generic one
		outs << "unknown error";
	}
	outs << " (" << Target.nID << ")";

	// Finish error message off and return it.
	outs << std::ends;
	acErrorBuffer[sizeof(acErrorBuffer) - 1] = '\0';
	return acErrorBuffer;
}


#endif
