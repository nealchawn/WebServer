/*
    Live Server on port 8888
	Support from Jon Erickson's Book
*/
#include<io.h>
#include<stdio.h>
// #include "C:\Program Files (x86)\Windows Kits\10\Include\10.0.17134.0\um\winsock2.h"
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<winsock2.h>

#include<sys/stat.h>
#include "defs.h"
// #include "hacking-network.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define PORT 8888
#define WEBROOT "./webroot"

//compile this program 
//gcc driver.c -o webserver -l "ws2_32" -fpermissive

void handle_connection(int, struct sockaddr_in *);
int get_file_size(int);

 void handle_connection(int sockfd, struct sockaddr_in *client_addr_ptr)
 {
	unsigned char *ptr, request[500], resource[500];
	int fd, length;
	
	length = recv_line(sockfd, request);
	
	printf("Got request from %s:%d \"%s\"\n", inet_ntoa(client_addr_ptr->sin_addr), ntohs(client_addr_ptr->sin_port), request);
	
	ptr = strstr(request, " HTTP/"); // Search for valid-looking request
	if(ptr == NULL)
		printf(" NOT HTTP!\n");
	else
	{
		*ptr = 0;
		ptr = NULL;
		if(strncmp(request, "GET ", 4) == 0) // GET request
			ptr = request+4; //ptr is the url
			printf("\n GET REQUEST \n");
		if(strncmp(request, "HEAD ", 5) == 0) // HEAD request
			ptr = request+5; //ptr is the url
			printf("\n HEAD REQUEST \n");
		if(ptr == NULL)
		{
			printf("\tUNKNOWN REUEST!\n");
		}
		else
		{
			if(ptr[strlen(ptr) - 1] == '/') // For resources ending with '/'
				strcat(ptr, "index.html"); //add index.html to the end.
			strcpy(resource, WEBROOT); //Begin resource with resource path.
			strcat(resource, ptr); // and join it with resource path.
			fd = open(resource, O_RDONLY, 0); // review Try to open file
			printf("\tOpening \'%s\'\t", resource);
			if(fd == -1)
			{
				printf(" 404 not found!!"); // 404 Not Found, Send 404 response
			}
			else
			{
				printf("\n 200 OK\n");
				send_string(sockfd, "HTTP/1.0 200 OK\r\n");
				send_string(sockfd, "Server: Chawn's webserver\r\n\r\n");
				if(ptr == request + 4)
				{
					if((length = get_file_size(fd)) == -1)
					{
						printf("error getting resource file size");
						return -1;
					}
					if((ptr = (unsigned char *)malloc(length)) == NULL)
					{
						printf("error allocating memory for reading resource");
						return -1;
					}
					read(fd, ptr, length); // Read the file into memory.
					//printf(ptr);
					send(sockfd, ptr, length, 0); // send it to socket.
					free(ptr);
				}
				close(fd); // Close the file.
			} // End if block for file found/not found.
		}// End if block for valid request.
	}// End if block for valid HTTP.
	//shutdown(sockfd, SHUT_RDWR); // close the socket gracefully
	//shutdown(sockfd, O_RDWR);
 }
 
int get_file_size(int fd)
{
	struct stat stat_struct;
	
	if(fstat(fd, &stat_struct) == -1)
		return -1;
	return (int)stat_struct.st_size;
}

int main(int argc , char *argv[])
{
    WSADATA wsa;
    SOCKET s , new_socket;
    struct sockaddr_in server , client;
    int c, yes=1;
    char *message;
 
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
     
    printf("Initialised.\n");
     
    //Create a socket
    if((s = socket(PF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
 
    printf("Socket created.\n");
    //setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int) == -1); //maybe?
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( PORT );
	
	memset(&(server.sin_zero), '\0', 8); //maybe?
    
	//Bind
    if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
     
    puts("Bind done");
 
    //Listen to incoming connections
    listen(s , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
     
    c = sizeof(struct sockaddr_in);
    char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
       fprintf(stdout, "Current working dir: %s\n", cwd);
    while(1)
    {
		
		new_socket = accept(s , (struct sockaddr *)&client, &c);
        //puts("Connection accepted");
		handle_connection(new_socket, &client);
        //Reply to the client
        
		//message = "Hello Client , I have received your connection. But I have to go now, bye\n";
        //send(new_socket , message , strlen(message) , 0);
    }
     
    if (new_socket == INVALID_SOCKET)
    {
        printf("accept failed with error code : %d" , WSAGetLastError());
        return 1;
    }
 
    closesocket(s);
    WSACleanup();
     
    return 0;
}