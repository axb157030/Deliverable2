// a server in the internet domain
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "log_s_functions.h"

int main(int argc, char *argv[])
{
	int sock, length, n, pid;
    	struct sockaddr_in serv_addr, cli_addr;
	socklen_t fromlen;
     	struct sockaddr_in server;
     	struct sockaddr_in from;
     	struct hostent *hp;
     	char buf[1024];
     	fd_set readfds;

	//Port Validation
     	if (argc < 2) {
        	fprintf(stderr, "ERROR: provide a hostname port");
     	}

	//Socket Creation
     	sock = socket(AF_INET, SOCK_DGRAM, 0);

	//Socket Validation
     	if (sock < 0){
		error("ERROR opening socket");
	}

	//Getting Host Address
	server.sin_family = AF_INET;
	hp = gethostbyname(argv[1]);
	
	//Address Validation
	if (hp==0){
		error("Unknown host");
	}
	
	bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	
   	server.sin_port = htons(3004);
   	length=sizeof(struct sockaddr_in);
	fromlen = sizeof(struct sockaddr_in);

	//UDP
     	while (1) {
        
		//Message recieving
        	n = recvfrom(sock,buf,1024,0,(struct sockaddr *)&from, &length);
         	if (n < 0){
	 		error("ERROR receiving from");
	 	}
	     	
		//If "echo.log" exists then appeand new log to the end of the file
		if (file = fopen("echo.log", "r")){  
			FILE *fPointer;
			fPointer = fopen("echo.log", "a");
			fprintf(fPointer, "\n"); 
			fprintf(fPointer, "%s", buf);
	 	}
	     
		//If "echo.log" does not exist then create a new file called "echo.log" and write in the log
	 	else{
			FILE *fPointer;
			fPointer = fopen("echo.log", "w"); 
			fprintf(fPointer, "%s", buf);
	 	}
		
		//Close pointer
	 	getch();
	 	fclose(fPointer);
     	}
	
     // close all sockets
     close(sock);
     return 0;
}

