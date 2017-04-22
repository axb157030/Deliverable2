// a server in the internet domain
// the pathname of the socket address is passed as an argument
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "server_functions.h"

int main(int argc, char *argv[])
{
     int sockfd, sock, newsockfd, portno, length, n, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     socklen_t fromlen;
     struct sockaddr_in server;
     struct sockaddr_in from;
     char buf[1024];
     fd_set readfds;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     sock = socket(AF_INET, SOCK_DGRAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     if (sock < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     length = sizeof(server);
     bzero(&server,length);
     server.sin_family=AF_INET;
     server.sin_addr.s_addr=INADDR_ANY;
     server.sin_port=htons(atoi(argv[1]));
     if (bind(sock,(struct sockaddr *)&server,length)<0) 
              error("ERROR on binding");
     fromlen = sizeof(struct sockaddr_in);
     pid = fork();
                     if (pid == 0)  
			{
                         //TCP
     		 listen(sockfd,5);
                 clilen = sizeof(cli_addr);
                 while (1) {
		     
                     newsockfd = accept(sockfd, 
                           (struct sockaddr *) &cli_addr, &clilen);
                     if (newsockfd < 0) 
                         error("ERROR on accept");
                     pid = fork();
                     if (pid < 0)
                         error("ERROR on fork");
                     if (pid == 0)  {
                         close(sockfd);
                         dostuff(newsockfd);
                         exit(0);
                         }
                         else close(newsockfd);
                     } /* end of while */
                     close(sockfd);
                     return 0; /* we never get here */
                         }
                         else 
			{
while (1) {
			//UDP
                         n = recvfrom(sock,buf,1024,0,(struct sockaddr *)&from,&fromlen);
                         if (n < 0) error("recvfrom");
                         write(1,"Received a datagram: ",21);
                         write(1,buf,n);
                         n = sendto(sock,"Got your message\n",17,
                                    0,(struct sockaddr *)&from,fromlen);
                         if (n  < 0) error("sendto");
                     }
			}
         // close all sockets
         close(sockfd);
         close(sock);
         close(newsockfd);

         return 0;
    }

