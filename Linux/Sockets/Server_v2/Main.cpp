/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
*/
#include "SocketSupport.h"

int main()
{

   zSocketInitialization();
   while (1) {

      zAcceptConnectionAndStartThread();

   } /* end of while */
   zSocketShutdown();

   return 0; /* we never get here */
}


