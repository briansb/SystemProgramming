#include "WinsockServer.h"


DWORD WINAPI EchoHandler(void* sd_)
{
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

	return nRetval;
}


bool EchoIncomingPackets(SOCKET sd)
{
	// Read data from client
	char acReadBuffer[kBufferSize];
	int nReadBytes;
	do {
		nReadBytes = recv(sd, acReadBuffer, kBufferSize, 0);
		if (nReadBytes > 0) {
			std::cout << "Received " << nReadBytes <<
				" bytes from client." << std::endl;

			int nSentBytes = 0;
			while (nSentBytes < nReadBytes) {
				int nTemp = send(sd, acReadBuffer + nSentBytes,
					nReadBytes - nSentBytes, 0);
				if (nTemp > 0) {
					std::cout << "Sent " << nTemp <<
						" bytes back to client." << std::endl;
					nSentBytes += nTemp;
				}
				else if (nTemp == SOCKET_ERROR) {
					return false;
				}
				else {
					// Client closed connection before we could reply to
					// all the data it sent, so bomb out early.
					std::cout << "Peer unexpectedly dropped connection!" <<
						std::endl;
					return true;
				}
			}
		}
		else if (nReadBytes == SOCKET_ERROR) {
			return false;
		}
	} while (nReadBytes != 0);

	std::cout << "Connection closed by peer." << std::endl;
	return true;
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