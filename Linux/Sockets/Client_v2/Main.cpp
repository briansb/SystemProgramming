#include "SocketSupport.h"
#include <thread>

//int main(int argc, char *argv[])
int main()
{
    srand((int)time(0));

    zSocketInitializationAndStart();

    int counter = 25;
    while (counter--) {
        int n = rand() % 1000 + 1;
        std::string strMessage = "Client data = " + std::to_string(n);

        zSendToServer(strMessage);
        zReceiveFromServer(strMessage);


        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        
    }
    zSocketShutdown();
    return 0;
} 
