#include "SocketSupport.h"

// These are Linux-specific routines
#if defined(__linux__)

int sockfd, res;
char buffer[256];

void error(const char *msg)
{
    perror(msg);
    exit(0);
}


// 
void zSocketInitializationAndStart() {
    //int sockfd, portno, n;
    int portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    //char buffer[256];
    portno = 4242;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)  error("ERROR opening socket");
    // server = gethostbyname("newtonia");
    // if (server == NULL) { fprintf(stderr,"ERROR, no such host\n"); exit(0); }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    //serv_addr.sin_addr.s_addr = inet_addr("192.168.1.3");
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // bcopy((char *)server->h_addr, 
    //      (char *)&serv_addr.sin_addr.s_addr,
    //      server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) error("ERROR connecting");
}

// 
void zSocketShutdown() {
    printf("Exited client loop\n");
    close(sockfd);
}

// sends message to server
void zSendToServer(std::string strMessageToServer) {
    bzero(buffer,256);
    strcpy(buffer, strMessageToServer.c_str());
    printf("Sent to server:  %s\n", buffer);
    res = write(sockfd,buffer,strlen(buffer));
    if (res < 0) error("ERROR writing to socket");
    return;
}

// receives return message from server, and checks for errors
void zReceiveFromServer(std::string strMessageToServer) {
    int received_bytes;
    bzero(buffer,256);
    res = read(sockfd,buffer,255);
    if (res < 0) error("ERROR reading from socket");
    // Match received data against transmitted data
	if (strncmp(buffer, strMessageToServer.c_str(), received_bytes) == 0) {
		printf("Received from server:  %s\n",buffer);
		printf("\tReply packet matches\n\n"); }
    
    return;
}











#endif