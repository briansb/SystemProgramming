/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

void *dostuff(void *); /* function prototype */

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

//int main(int argc, char *argv[])
int main()
{
   int sockfd, portno;
   int client_sock, *new_sock;

   socklen_t clilen;
   struct sockaddr_in serv_addr, cli_addr;

   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) error("ERROR opening socket");

   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 4242;
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   if (bind(sockfd, (struct sockaddr *) &serv_addr,
         sizeof(serv_addr)) < 0) 
         error("ERROR on binding");

   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   while ( (client_sock = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen))) {
      if (client_sock < 0) error("ERROR on accept");

      printf("Accepted connection\n");

      pthread_t sniffer_thread;
		new_sock = (int*)malloc(1);
		*new_sock = client_sock;

      if( pthread_create( &sniffer_thread , NULL ,  dostuff , (void*) new_sock) < 0)
		{
			perror("could not create thread");
			return 1;
		}

      puts("Handler assigned");

      // pid = fork();
      // if (pid < 0) error("ERROR on fork");
      // if (pid == 0)  {
      //    close(sockfd);
      //    dostuff(newsockfd);
      //    exit(0);
      // }
      //else close(newsockfd);



   } /* end of while */
   close(sockfd);
   return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void *dostuff (void *socket_desc)
{
   int n;
   int sock = *(int*)socket_desc;
   char buffer[256];

   while (1) {   
    bzero(buffer,256);
    n = read(sock,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);
    n = write(sock,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");
   }
} 
