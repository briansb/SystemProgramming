#include "support.h"

#include <iostream>
#include <strstream>

#pragma comment (lib, "Ws2_32.lib")

#if !defined(_WINSOCK2API_) 
// Winsock 2 header defines this, but Winsock 1.1 header doesn't.  In
// the interest of not requiring the Winsock 2 SDK which we don't really
// need, we'll just define this one constant ourselves.
#define SD_SEND 1
#endif

const int kBufferSize = 1024;

static struct ErrorEntry {
	int nID;
	const char* pcMessage;
	ErrorEntry(int id, const char* pc = 0) : nID(id), pcMessage(pc) 	{}
	bool operator<(const ErrorEntry& rhs) const 	{ return nID < rhs.nID; 	}} 

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


void InitializeWinsockAndConnectToServer(SOCKET &sd) {
	// Start Winsock up
	// The WSAStartup function must be the first Windows Sockets function called by an application or DLL.
	// It specifies the version of Winsock that this application needs.  We need version 1.1
	WSAData wsaData;
	int err;
	if ((err = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) { std::cerr << "WSAStartup() returned error code " << err << "." << std::endl; exit(1); return; }

	const char *pchost = "127.0.0.1";
	int port = 4242;
	// Find the server's address
	u_long nRemoteAddress = LookupAddress(pchost);
	if (nRemoteAddress == INADDR_NONE) { std::cerr << std::endl << WSAGetLastErrorMessage("Lookup server address error") << std::endl; exit(1); return; }
	in_addr Address;
	memcpy(&Address, &nRemoteAddress, sizeof(u_long));
	std::cout << inet_ntoa(Address) << ":" << port << std::endl;

	// Connect to the server
	sd = EstablishConnection(nRemoteAddress, htons(port));
	if (sd == INVALID_SOCKET) { std::cerr << std::endl << WSAGetLastErrorMessage("connect to server") << std::endl; exit(1); return; }
	std::cout << "Connected, socket " << sd << std::endl;
	return;
}
// Given an address string, determine if it's a dotted-quad IP address or a domain address.  
// If the latter, ask DNS to resolve it.  In either case, return resolved IP address.  If we fail, we return INADDR_NONE.
u_long LookupAddress(const char* pcHost)
{
	u_long nRemoteAddr = inet_addr(pcHost);
	if (nRemoteAddr == INADDR_NONE) {
		// pcHost isn't a dotted IP, so resolve it through DNS
		hostent* pHE = gethostbyname(pcHost);
		if (pHE == 0) { return INADDR_NONE; }
		nRemoteAddr = *((u_long*)pHE->h_addr_list[0]);
	}
	return nRemoteAddr;
}

// Connects to a given address, on a given port, both of which must be in network byte order.  
// Returns newly-connected socket if we succeed, or INVALID_SOCKET if we fail.
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
const char* WSAGetLastErrorMessage(const char* pcMessagePrefix, 	int nErrorID /* = 0 */) {
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

void CloseConnectionAndShutdownWinsock(SOCKET &sd) {
	// Shut connection down
	if (ShutdownConnection(sd)) { std::cout << "Connection is down." << std::endl; }
	else { std::cout << std::endl << WSAGetLastErrorMessage("Shutdown connection") << std::endl; }

	// Shut Winsock down
	WSACleanup();
}
bool ShutdownConnection(SOCKET sd)
{
	if (shutdown(sd, SD_SEND) == SOCKET_ERROR) { return false; }
	char acReadBuffer[kBufferSize];
	while (1) {
		int nNewBytes = recv(sd, acReadBuffer, kBufferSize, 0);
		if (nNewBytes == SOCKET_ERROR) {
			return false;
		}
		else if (nNewBytes != 0) {
			std::cerr << std::endl << "FYI, received " << nNewBytes <<
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

