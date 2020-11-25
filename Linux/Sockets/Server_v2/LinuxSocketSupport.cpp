#include "SocketSupport.h"

// These are Linux-specific routines
#if defined(__linux__)


static int newsockfd;
static int sockfd;
static int pid;
static socklen_t clilen;
static struct sockaddr_in cli_addr;

// Create socket and start listener
void zSocketInitialization() {
    int portno;
    struct sockaddr_in serv_addr;
    printf("Establishing the listener...\n");
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
}

// Close socket
void zSocketShutdown() {
    close(sockfd);
}

// receives message from client, and checks for errors
void zAcceptConnectionAndStartThread() {
    printf("Waiting for connections...\n");
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) error("ERROR on accept");
    char *some_addr;
    some_addr = inet_ntoa(cli_addr.sin_addr); // return the IP
    //printf("%s\n", some_addr); // prints "10.0.0.1"
    printf("Accepted connection from %s:%d\n", some_addr, cli_addr.sin_port);
    pid = fork();
    if (pid < 0) error("ERROR on fork");
    if (pid == 0)  {
        close(sockfd);
        ReceiveAndSendToClient(newsockfd);
        exit(0);
    }
    else close(newsockfd);


}


// There is a separate instance of this function for each connection.  
// It handles all communicationonce a connnection has been established.
void ReceiveAndSendToClient(int sock)
{
   int n;
   char buffer[256];

   while (1) {   
      bzero(buffer,256);
      n = read(sock,buffer,255);
      if (n < 0) error("ERROR reading from socket");
      printf("Received from client:  %s\n",buffer);

      printf("Sent to client:  %s\n\n", buffer);
      n = write(sock,buffer,strlen(buffer));
      if (n < 0) error("ERROR writing to socket");
   }
} 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}


#endif