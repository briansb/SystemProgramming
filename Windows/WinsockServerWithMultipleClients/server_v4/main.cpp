// https://tangentsoft.net/wskfaq/
// excellent resource for both client and server

#include "SocketSupport.h"




int main()
{
	
	zSocketInitialization();
	int stop = 1;
	while (stop) {
		std::cout << "Before zStart" << std::endl;
		zAcceptConnectionAndStartThread(stop);
		std::cout << "After zStart" << std::endl;
	}
	
	zSocketShutdown();
	return 0;
}


