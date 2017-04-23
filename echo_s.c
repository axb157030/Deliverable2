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
#include "echo_s_functions.h"

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

//NEW//
int sockfd_log;
//NEW//

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     sock = socket(AF_INET, SOCK_DGRAM, 0);

//NEW//
sockfd_log = socket(AF_INET, SOCK_DGRAM, 0);
//NEW//

     if (sockfd < 0) 
        error("ERROR opening socket");
     if (sock < 0)
        error("ERROR opening socket");

//NEW//
if (sockfd_log < 0)
     error("ERROR opening socket");
//NEW//

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

//NEW//
     length = sizeof(server);
     bzero(&server,length);
     server.sin_family=AF_INET;
     server.sin_addr.s_addr=INADDR_ANY;
     //TEMPORARY//
     server.sin_port=htons(3004);
     if (bind(sockfd_log,(struct sockaddr *)&server,length)<0) 
              error("ERROR on binding line 73");
//NEW//

     fromlen = sizeof(struct sockaddr_in);

//NEW//
     if((pid = fork())==-1) error("ERROR on fork");
//NEW//

     if (pid == 0)
     {
         //TCP
         listen(sockfd,5);
         clilen = sizeof(cli_addr);
         while (1) {
             newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
             if (newsockfd < 0) 
                 error("ERROR on accept");
             pid = fork();
             if (pid < 0)
                 error("ERROR on fork");
             if (pid == 0)  {
                 close(sockfd);

                 //NEW//
    int childpid_date, index, i, sock;
    char date_buf[256];
    char fromEcho_c[256];
    char toLog_s[1024];
    int date_pipe[2];
    pipe(date_pipe);

   if((childpid_date = fork()) == -1) error("ERROR on fork");
   if(childpid_date == 0)
   {
       dup2(date_pipe[1], 1);
       close(date_pipe[0]); close(date_pipe[1]);
       char *date_args[] = {"date", "+\"%F %T\"", NULL};
       execvp(*date_args, date_args);
       exit(0);
   }
   n = read(date_pipe[0], date_buf, sizeof(date_buf));
   if(n < 0) error("ERROR reading from socket");
   close(date_pipe[0]); close(date_pipe[1]);

   // read from client
   bzero(fromEcho_c,256);
   n = read(newsockfd,fromEcho_c,255);

   if (n < 0){
      error("ERROR reading from socket");
   }
   // display the message
   printf("Here is the message: %s\n",fromEcho_c);
//NEW//

   /// reply
   n = write(newsockfd,"I got your message",18);
   if (n < 0){
       error("ERROR writing to socket");
   }

//NEW//
   for(index = 0; date_buf[index] != '\0' && index < strlen(date_buf); index++)
   {
	toLog_s[index] = date_buf[index];
   }
   toLog_s[index] = '\t';
   for(i = 0, index=index+1; fromEcho_c[i] != '\0' && i < strlen(fromEcho_c); i++, index++)
   {
	toLog_s[index] = fromEcho_c[i];
   }   
   toLog_s[index] = '\0';
   sendto(sockfd_log, toLog_s, strlen(toLog_s), 0, (struct sockaddr *)&from, fromlen);
//NEW//


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

