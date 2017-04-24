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
     int *sockfd, *sock,*newsockfd, portno, length, n, pid, noport, port, maxDescriptor;
     maxDescriptor = -1;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     socklen_t fromlen;
     struct sockaddr_in server;
     struct sockaddr_in from;
     char buf[1024];
     fd_set readfds;
     noport = argc - 2;
     int sockfd_log;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
    // sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // sock = socket(AF_INET, SOCK_DGRAM, 0);
    sockfd = (int *) malloc(noport * sizeof(int));
    sock = (int *) malloc(noport * sizeof(int));

sockfd_log = socket(AF_INET, SOCK_DGRAM, 0);
if(sockfd_log <0)
error("Error opening Socket");

     for(port = 0; port < noport; port++) {
	noport = atoi(argv[port + 2]);
	sockfd[port] = socket(AF_INET, SOCK_STREAM, 0);
        sock[port] = socket(AF_INET, SOCK_DGRAM, 0);
     if (sockfd[port] < 0) 
        error("ERROR opening socket");
     if (sock[port] < 0)
        error("ERROR opening socket");

//if (sockfd_log < 0)
    // error("ERROR opening socket");

     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd[port], (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     length = sizeof(server);
     bzero(&server,length);
     server.sin_family=AF_INET;
     server.sin_addr.s_addr=INADDR_ANY;
     server.sin_port=htons(atoi(argv[1]));
     if (bind(sock[port],(struct sockaddr *)&server,length)<0) 
              error("ERROR on binding");
	if(sockfd[port] > maxDescriptor)
	maxDescriptor = sockfd[port];
	if(sock[port] > maxDescriptor)
	maxDescriptor = sock[port];

}
     length = sizeof(server);
     bzero(&server,length);
     server.sin_family=AF_INET;
     server.sin_addr.s_addr=INADDR_ANY;
     //////////////////////////////TEMPORARY: using port 3004 instead of 9999//////////////////////////////
     server.sin_port=htons(3004);
     if (bind(sockfd_log,(struct sockaddr *)&server,length)<0) 
              error("ERROR on binding line 73");

     fromlen = sizeof(struct sockaddr_in);

     if((pid = fork())==-1) error("ERROR on fork");

     if (pid == 0)
     {
         //TCP
	 for(port = 0; port < noport; port++){
         listen(sockfd[port],5);
         clilen = sizeof(cli_addr);
         while (1) {
             newsockfd[port] = accept(sockfd[port], (struct sockaddr *) &cli_addr, &clilen);
             if (newsockfd[port] < 0) 
                 error("ERROR on accept");
             pid = fork();
             if (pid < 0)
                 error("ERROR on fork");
             if (pid == 0)  {
                 close(sockfd[port]);

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
   n = read(newsockfd[port],fromEcho_c,255);

   if (n < 0){
      error("ERROR reading from socket");
   }
   // display the message
   printf("Here is the message: %s\n",fromEcho_c);

   /// reply
   n = write(newsockfd[port],"I got your message",18);
   if (n < 0){
       error("ERROR writing to socket");
   }

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


                 exit(0);
             }
             else close(newsockfd[port]);
         } /* end of while */
         close(sockfd[port]);
	 }
             return 0; /* we never get here */
}
                         else 
			{
while (1) {
			//UDP
			for(port = 0; port < noport; port++){
                         n = recvfrom(sock[port],buf,1024,0,(struct sockaddr *)&from,&fromlen);
                         if (n < 0) error("recvfrom");
                         write(1,"Received a datagram: ",21);
                         write(1,buf,n);
                         n = sendto(sock[port],"Got your message\n",17,
                                    0,(struct sockaddr *)&from,fromlen);
                         if (n  < 0) error("sendto");}
                     }
			}
         // close all sockets
         close(sockfd);
         close(sock);
         close(newsockfd);

         return 0;
    }

