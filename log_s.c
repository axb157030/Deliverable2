// a server in the internet domain
#include <stdio.h>
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

     if (argc < 2) {
        fprintf(stderr, "ERROR: provide a hostname port");
     }

     sock = socket(AF_INET, SOCK_DGRAM, 0);

     if (sock < 0)
        error("ERROR opening socket");

server.sin_family = AF_INET;
   hp = gethostbyname(argv[1]);
   if (hp==0) error("Unknown host");
bcopy((char *)hp->h_addr, 
        (char *)&server.sin_addr,
         hp->h_length);
   server.sin_port = htons(3004);
   length=sizeof(struct sockaddr_in);

     fromlen = sizeof(struct sockaddr_in);

     while (1) {
         //UDP
         n = recvfrom(sock,buf,1024,0,(struct sockaddr *)&from, &length);
         if (n < 0) error("ERROR receiving from");
	 //WRITE BUF CONTENTS TO echo.log HERE//
     }
     // close all sockets
     close(sock);

     return 0;
}

