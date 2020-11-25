/***********************************************************************
 basic-client.cpp - Implements a fairly basic Winsock client program
	that initiates a blocking TCP connection to an echo server, sends
	some data, waits for and receives the reply, and dies.

 Compiling:
	VC++: cl -GX basic-client.cpp main.cpp ws-util.cpp wsock32.lib
***********************************************************************/

#include "SocketSupport.h"

// These are Windows-specific routines
#if defined(_WIN32) || defined(_WIN64)

SOCKET sd;

const char* WSAGetLastErrorMessage(const char* pcMessagePrefix, int nErrorID = 0);


std::string PrintBuffer(char c[], int n) {
	std::string s;
	for (int i = 0; i < n; ++i) 	s += c[i];
	return s;
}

void zSocketInitializationAndStart() {
	// Start Winsock up
	WSAData wsaData;
	int nCode;
	if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) { std::cout << "WSAStartup() returned error code " << nCode << "." << std::endl; return; }

	const char* pcHost = "127.0.0.1";
	int nPort = 4242;
	// Find the server's address
	// Determine if it's a dotted-quad IP address or a domain address.  
	// If domain, query DNS
	std::cout << "Looking up address..." << std::flush;
	u_long nRemoteAddress = inet_addr(pcHost);
	if (nRemoteAddress == INADDR_NONE) {
		// pcHost isn't a dotted IP, so resolve it through DNS
		hostent* pHE = gethostbyname(pcHost);
		if (pHE == 0) { std::cout << std::endl << WSAGetLastErrorMessage("lookup address") << std::endl; return; }
		nRemoteAddress = *((u_long*)pHE->h_addr_list[0]);
	}
	in_addr Address;
	memcpy(&Address, &nRemoteAddress, sizeof(u_long));
	std::cout << inet_ntoa(Address) << ":" << nPort << std::endl;

	// Connect to the server
	// Connects to a given address, on a given port, 
	// both of which must be in network byte order.  
	std::cout << "Connecting to remote host..." << std::endl;
	// Create a stream socket
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd != INVALID_SOCKET) {
		sockaddr_in sinRemote;
		sinRemote.sin_family = AF_INET;
		sinRemote.sin_addr.s_addr = (u_long)nRemoteAddress;
		sinRemote.sin_port = (u_short)htons(nPort);
		if (connect(sd, (sockaddr*)&sinRemote, sizeof(sockaddr_in)) == SOCKET_ERROR) { std::cout << std::endl << WSAGetLastErrorMessage("socket error") << std::endl; 	exit(1); }
	}
	std::cout << "Connected.  Socket " << sd << "." << std::endl;
	return;
}

void zSocketShutdown() {
	// Shut connection down
	std::cout << "Shutting connection down..." << std::endl;

	//if (ShutdownConnection()) { std::cout << "Connection is closed" << std::endl; }
	//else { std::cout << std::endl << WSAGetLastErrorMessage("Shutdown connection") << std::endl; }

	// Disallow any further data sends.  Required for graceful exit.
	if (shutdown(sd, SD_SEND) == SOCKET_ERROR) std::cout << WSAGetLastErrorMessage("shutdown error") << std::endl;

	// Receive any extra data still sitting on the socket.  
	// After all data is received, this call will block until the remote host
	// acknowledges the TCP control packet sent by the shutdown above.
	// A zero will be received signaling that the remote host has closed its side of the connection.
	char acReadBuffer[kBufferSize];
	while (1) {
		int nNewBytes = recv(sd, acReadBuffer, kBufferSize, 0);
		if (nNewBytes == SOCKET_ERROR) std::cout << WSAGetLastErrorMessage("clear buffer error") << std::endl;
		else if (nNewBytes != 0) std::cout << "Received " << nNewBytes << " unexpected bytes during shutdown." << std::endl;
		else break;
	}

	// Close the socket.
	if (closesocket(sd) == SOCKET_ERROR) std::cout << WSAGetLastErrorMessage("close socket error") << std::endl;

	// Shut Winsock down
	WSACleanup();
}

void zSendToServer(std::string s) {
	// Send the string to the server
	const size_t strMessageLen = s.length();
	std::cout << "Sent to server:  " << s << std::endl;
	if (send(sd, s.c_str(), strMessageLen, 0) == SOCKET_ERROR) { std::cout << WSAGetLastErrorMessage("Send to server") << std::endl; exit(1); }
	return;
}

void zReceiveFromServer(std::string s) {
	// Read reply from server
	const size_t strMessageLen = s.length();
	char acReadBuffer[kBufferSize];
	size_t nTotalBytes = 0;
	while (nTotalBytes < strMessageLen) {
		int nNewBytes = recv(sd, acReadBuffer + nTotalBytes, kBufferSize - nTotalBytes, 0);
		if (nNewBytes == SOCKET_ERROR) return;
		else if (nNewBytes == 0) { std::cout << "Connection closed by peer." << std::endl; return; }
		nTotalBytes += nNewBytes; }

	if (nTotalBytes >= kBufferSize) { std::cout << "Received data size bigger than buffer size" << std::endl; return; }
	if (nTotalBytes == 0) { std::cout << "Server unexpectedly closed connection" << std::endl; return; }
	if (nTotalBytes < 0) { std::cout << WSAGetLastErrorMessage("ReceiveFromServer") << std::endl; return; }
	// Match received data against transmitted data
	if (strncmp(acReadBuffer, s.c_str(), nTotalBytes) == 0) {
		std::cout << "Received from server:  " << PrintBuffer(acReadBuffer, nTotalBytes) << std::endl;
		std::cout << "\tReply packet matches" << std::endl << std::endl; }
	else std::cout << "Data from server does not match transmitted data" << std::endl;

	return;
}



// List of Winsock error constants mapped to an interpretation string.
// Note that this list must remain sorted by the error constants'
// values, because we do a binary search on the list when looking up
// items.
static struct ErrorEntry {
	int nID;
	const char* pcMessage;
	ErrorEntry(int id, const char* pc = 0) : nID(id), pcMessage(pc) {}
	bool operator<(const ErrorEntry& rhs) const {
		return nID < rhs.nID;
	}
}

gaErrorList[] = {
	ErrorEntry(0,                  "No error"),
	ErrorEntry(WSAEINTR,           "Interrupted system call"),
	ErrorEntry(WSAEBADF,           "Bad file number"),
	ErrorEntry(WSAEACCES,          "Permission denied"),
	ErrorEntry(WSAEFAULT,          "Bad address"),
	ErrorEntry(WSAEINVAL,          "Invalid argument"),
	ErrorEntry(WSAEMFILE,          "Too many open sockets"),
	ErrorEntry(WSAEWOULDBLOCK,     "Operation would block"),
	ErrorEntry(WSAEINPROGRESS,     "Operation now in progress"),
	ErrorEntry(WSAEALREADY,        "Operation already in progress"),
	ErrorEntry(WSAENOTSOCK,        "Socket operation on non-socket"),
	ErrorEntry(WSAEDESTADDRREQ,    "Destination address required"),
	ErrorEntry(WSAEMSGSIZE,        "Message too long"),
	ErrorEntry(WSAEPROTOTYPE,      "Protocol wrong type for socket"),
	ErrorEntry(WSAENOPROTOOPT,     "Bad protocol option"),
	ErrorEntry(WSAEPROTONOSUPPORT, "Protocol not supported"),
	ErrorEntry(WSAESOCKTNOSUPPORT, "Socket type not supported"),
	ErrorEntry(WSAEOPNOTSUPP,      "Operation not supported on socket"),
	ErrorEntry(WSAEPFNOSUPPORT,    "Protocol family not supported"),
	ErrorEntry(WSAEAFNOSUPPORT,    "Address family not supported"),
	ErrorEntry(WSAEADDRINUSE,      "Address already in use"),
	ErrorEntry(WSAEADDRNOTAVAIL,   "Can't assign requested address"),
	ErrorEntry(WSAENETDOWN,        "Network is down"),
	ErrorEntry(WSAENETUNREACH,     "Network is unreachable"),
	ErrorEntry(WSAENETRESET,       "Net connection reset"),
	ErrorEntry(WSAECONNABORTED,    "Software caused connection abort"),
	ErrorEntry(WSAECONNRESET,      "Connection reset by peer"),
	ErrorEntry(WSAENOBUFS,         "No buffer space available"),
	ErrorEntry(WSAEISCONN,         "Socket is already connected"),
	ErrorEntry(WSAENOTCONN,        "Socket is not connected"),
	ErrorEntry(WSAESHUTDOWN,       "Can't send after socket shutdown"),
	ErrorEntry(WSAETOOMANYREFS,    "Too many references, can't splice"),
	ErrorEntry(WSAETIMEDOUT,       "Connection timed out"),
	ErrorEntry(WSAECONNREFUSED,    "Connection refused"),
	ErrorEntry(WSAELOOP,           "Too many levels of symbolic links"),
	ErrorEntry(WSAENAMETOOLONG,    "File name too long"),
	ErrorEntry(WSAEHOSTDOWN,       "Host is down"),
	ErrorEntry(WSAEHOSTUNREACH,    "No route to host"),
	ErrorEntry(WSAENOTEMPTY,       "Directory not empty"),
	ErrorEntry(WSAEPROCLIM,        "Too many processes"),
	ErrorEntry(WSAEUSERS,          "Too many users"),
	ErrorEntry(WSAEDQUOT,          "Disc quota exceeded"),
	ErrorEntry(WSAESTALE,          "Stale NFS file handle"),
	ErrorEntry(WSAEREMOTE,         "Too many levels of remote in path"),
	ErrorEntry(WSASYSNOTREADY,     "Network system is unavailable"),
	ErrorEntry(WSAVERNOTSUPPORTED, "Winsock version out of range"),
	ErrorEntry(WSANOTINITIALISED,  "WSAStartup not yet called"),
	ErrorEntry(WSAEDISCON,         "Graceful shutdown in progress"),
	ErrorEntry(WSAHOST_NOT_FOUND,  "Host not found"),
	ErrorEntry(WSANO_DATA,         "No host data of that type was found")
};
const int kNumMessages = sizeof(gaErrorList) / sizeof(ErrorEntry);

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