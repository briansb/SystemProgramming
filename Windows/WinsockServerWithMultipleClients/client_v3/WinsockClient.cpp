/***********************************************************************
 basic-client.cpp - Implements a fairly basic Winsock client program
	that initiates a blocking TCP connection to an echo server, sends
	some data, waits for and receives the reply, and dies.

 Compiling:
	VC++: cl -GX basic-client.cpp main.cpp ws-util.cpp wsock32.lib
***********************************************************************/
#include "WinsockClient.h"

std::string BufferToString(const char CharBuffer[], size_t buffer_size) {
	int i = 0;
	std::string s;
	while (i < buffer_size) {
		s += CharBuffer[i++];
	}
	return s;
}


// Send the string to the server
bool SendMessage(SOCKET sd, const char* s)
{
	const size_t strMessageLen = strlen(s);
	//size_t strMessageLen = strMessage.length();
	std::cout << "Sent message:  " << BufferToString(s, strMessageLen) << std::endl;
	//std::cout << "Length:  " << strMessageLen << " bytes" << std::endl;
	if (send(sd, s, (int)strMessageLen, 0) != SOCKET_ERROR) return true;
	return false;
}

// Read reply from server
int ReadReply(SOCKET sd, const char* s)
{
	const size_t strMessageLen = strlen(s);
	char acReadBuffer[kBufferSize];
	size_t nTotalBytes = 0;
	while (nTotalBytes < strMessageLen) {
		int nNewBytes = recv(sd, acReadBuffer + nTotalBytes, 
			kBufferSize - (int)nTotalBytes, 0);
		if (nNewBytes == SOCKET_ERROR) {
			return -1;
		}
		else if (nNewBytes == 0) {
			std::cout << "Connection closed by peer." << std::endl;
			return 0;
		}
		nTotalBytes += nNewBytes;
	}

	// Check data for sanity
	if (strncmp(acReadBuffer, s, nTotalBytes) == 0) {
		//std::cout << "Received message:  " << BufferToString(acReadBuffer, nTotalBytes) << std::endl;
		//std::cout << "Length:  " << nTotalBytes << " bytes." << std::endl;
	}
	else {
		std::cout << "Length mismatch between sent and received data" << std::endl;
	}
	return (int)nTotalBytes;
}



SOCKET EstablishConnection(u_long nRemoteAddr, u_short nPort)
{
	// Create a stream socket
	SOCKET sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd != INVALID_SOCKET) {
		sockaddr_in sinRemote;
		sinRemote.sin_family = AF_INET;
		sinRemote.sin_addr.s_addr = nRemoteAddr;
		sinRemote.sin_port = nPort;
		if (connect(sd, (sockaddr*)&sinRemote, sizeof(sockaddr_in)) ==
			SOCKET_ERROR) {
			sd = INVALID_SOCKET;
		}
	}

	return sd;
}

u_long LookupAddress(const char* pcHost)
{
	u_long nRemoteAddr = inet_addr(pcHost);
	if (nRemoteAddr == INADDR_NONE) {
		// pcHost isn't a dotted IP, so resolve it through DNS
		hostent* pHE = gethostbyname(pcHost);
		if (pHE == 0) {
			return INADDR_NONE;
		}
		nRemoteAddr = *((u_long*)pHE->h_addr_list[0]);
	}

	return nRemoteAddr;
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
