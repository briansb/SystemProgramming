// https://www.binarytides.com/server-client-example-c-sockets-linux/

#include<stdio.h>
#include<string.h>	//strlen
#include<stdlib.h>	//strlen
#include <string>
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include <pthread.h> //for threading , link with lpthread
#include <iostream>

//the thread function
void *connection_handler(void *);

std::string CharToString(char message[], int n) {
	int i = 0;
	std::string s;
	while (i < n) {
		s += message[i++];
	}
	return s;
}

struct sockaddr_in server , client;

//int main(int argc , char *argv[])
int main()
{
	int socket_desc , client_sock , c , *new_sock;

	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 4242 );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");
	
	//Listen
	puts("Listening for incoming connections...");
	listen(socket_desc , 3);
	c = sizeof(struct sockaddr_in);
	
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{

		printf("Accepted connection from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

		
		pthread_t sniffer_thread;
		new_sock = (int*)malloc(1);
		*new_sock = client_sock;
		
		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
		{
			perror("could not create thread");
			return 1;
		}
		
		//Now join the thread , so that we dont terminate before the thread
		//pthread_join( sniffer_thread , NULL);
		puts("Handler assigned");
	}
	
	if (client_sock < 0)
	{
		perror("accept failed");
		return 1;
	}
	
	return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
	//Get the socket descriptor
	int sock = *(int*)socket_desc;
	int read_size;
	char client_message[2000];
	
	// std::string str1("Greetings! I am your connection handler\n");
	// const char *message1  = str1.c_str();
	// std::string str2("This is a message from the server \n");
	// const char *message2  = str2.c_str();
	
	
	//Send some messages to the client
	//message = "Greetings! I am your connection handler\n";
	//write(sock , message1 , strlen(message1));
	
	//message = "Now type something and i shall repeat what you type \n";
	//write(sock , message2 , strlen(message2));
	
	//Receive a message from client
	std::string strMessage;
	int index;
	std::string ClientID;
	std::string data;
	while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
	{
		strMessage = CharToString(client_message, read_size);
		index = strMessage.find_first_of(" ");
		ClientID = strMessage.substr(0,index);
		data = strMessage.substr(index+1,std::string::npos);
		std::cout << "Client PID " << ClientID << " sent data value " << data << std::endl;

		//Send the value, socket number, and PID back to client
		strMessage = data + " " + inet_ntoa(client.sin_addr) + ":" 
					+ std::to_string(ntohs(client.sin_port)) + 
					" " + ClientID;
		write(sock , strMessage.c_str() , strMessage.length());
		std::cout << "Sent data value " << data << " from server connection " <<
					inet_ntoa(client.sin_addr) << ":" << std::to_string(ntohs(client.sin_port)) <<
					" to client PID " << ClientID << std::endl << std::endl;				
	}
	
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}
		
	//Free the socket pointer
	free(socket_desc);
	
	return 0;
}
