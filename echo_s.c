// echo_s.c
// a server in the internet domain
// the hostname port and at least 1, but up to 3 port numbers are passed as arguments
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
     int *sockfd, *sock,*newsockfd,sockfd_log, portno, length, n, pid, noport, port, maxDescriptor;
     maxDescriptor = -1;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     socklen_t fromlen;
     struct sockaddr_in server;
     struct sockaddr_in from;
     char buf[1024];
     fd_set readfds;
	
     // if no port is provided, print an error
     if (argc < 2 || argc > 5) {
         fprintf(stderr,"ERROR, no port or too many ports provided\n");
         exit(1);
     }
	
     // make sockets for TCP clients, UDP clients, and the log server; print an error if it fails
     //sockfd = makeSocket(SOCK_STREAM);
     //sock = makeSocket(SOCK_DGRAM);
     sockfd_log = makeSocket(SOCK_DGRAM);
	if(sockfd_log <0)
		error("Error opening Socket");
	
	for(port = 0; port < noport; port++) {
	noport = atoi(argv[port]);
	sockfd[port] = socket(AF_INET, SOCK_STREAM, 0);
        sock[port] = socket(AF_INET, SOCK_DGRAM, 0);
     if (sockfd[port] < 0) 
        error("ERROR opening socket");
     if (sock[port] < 0)
        error("ERROR opening socket");


     // bind the TCP socket and output an error if it fails
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[port +2]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd[port], (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     length = sizeof(server);
    
     // bind the UDP socket and output an error if it fails
     bzero(&server,length);
     server.sin_family=AF_INET;
     server.sin_addr.s_addr=INADDR_ANY;
     server.sin_port=htons(atoi(argv[1]));
     if (bind(sock,(struct sockaddr *)&server,length)<0) 
              error("ERROR on binding");
		if(sockfd[port] > maxDescriptor)
	maxDescriptor = sockfd[port];
	if(sock[port] > maxDescriptor)
	maxDescriptor = sock[port];
	}
     length = sizeof(server);

     // bind the log server socket to port 9999 and output an error if it fails
     bzero(&server,length);
     server.sin_family=AF_INET;
     //Passes the second argument as the logip
     server.sin_addr.s_addr=($2);
     server.sin_port=htons(9999);
     if (bind(sockfd_log,(struct sockaddr *)&server,length)<0) 
              error("ERROR on binding line 73");
     fromlen = sizeof(struct sockaddr_in);

     // fork a child and output an error if it fails
     if((pid = fork())==-1) error("ERROR on fork");

     // the child will respond to the TCP connections
     if (pid == 0)
     {
	 // listen to the TCP socket
	      for(port = 0; port < noport; port++){
         listen(sockfd[port],5);
         clilen = sizeof(cli_addr);

	 // create an infinite loop for continuity
	 while (1) {
             // accept the TCP socket and output an error if it fails
             newsockfd[port] = accept(sockfd[port], (struct sockaddr *) &cli_addr, &clilen);
             if (newsockfd[port] < 0) 
                 error("ERROR on accept");
             // fork a child and output an error if it fails
             pid = fork();
             if (pid < 0)
                 error("ERROR on fork");
		 
    		 int date_pipe[2];
    		 pipe(date_pipe);

		 // fork a child for the date execution and output an error if it fails
   		 if((childpid_date = fork()) == -1) error("ERROR on fork");
   		 
		 // the child will perform the date execution
		 if(childpid_date == 0)
   		 {
	 	     // replace the stdout with date_pipe
       		     dup2(date_pipe[1], 1);
			 
		     // close all the pipes
       		     close(date_pipe[0]);
		     close(date_pipe[1]);
		
		     // execute 'date +"%F %T"'
       		     char *date_args[] = {"date", "+\"%F %T\"", NULL};
       		     execvp(*date_args, date_args);
			 
		     // exit the child process
       		     exit(0);
   		 }
		     
		 // read from the date_pipe and place the contents into the date_buf; output an error if it fails
   		 n = read(date_pipe[0], date_buf, sizeof(date_buf));
   		 if(n < 0) error("ERROR reading from socket");
		     
		 // close all the pipes
   		 close(date_pipe[0]); close(date_pipe[1]);

   		 // read from client, output an error if it fails
   		 bzero(fromEcho_c,256);
   		 n = read(newsockfd[port],fromEcho_c,255);
   		 if (n < 0){
      		     error("ERROR reading from socket");
           }
           // display the message sent from the client to the stdout
           printf("Here is the message: %s\n",fromEcho_c);

	   // reply to the client, output an error if it fails
	   n = write(newsockfd[port],fromEcho_c,18);
   	   if (n < 0){
               error("ERROR writing to socket");
   	   }

   	   // toLog_s = "timeAndDate"
   	   for(index = 0; date_buf[index] != '\0' && index < strlen(date_buf); index++)
	       toLog_s[index] = date_buf[index];
		     
   	   // toLog_s += "	"
   	   toLog_s[index] = '\t';
		     
   	   // toLog_s += "messageFromClient"
   	   for(i = 0, index=index+1; fromEcho_c[i] != '\0' && i < strlen(fromEcho_c); i++, index++)
	       toLog_s[index] = fromEcho_c[i];
		     
   	   // toLog_s += ""
   	   toLog_s[index] = '\0';
		     
   	         // send toLog_s to the log server
   	         n = sendto(sockfd_log, toLog_s, strlen(toLog_s), 0, (struct sockaddr *)&from, fromlen);
		 if (n < 0){
                     error("ERROR writing to socket LOG");
   	   	 }

   	         // exit the child process
   	         exit(0);
             }
		 
             // the parent closes the socket; to continue the server
             else close(newsockfd[port]);
	 } /* end of while */
	     
	 // close the socket
         close(sockfd[port]);
	      }
         return 0; /* we never get here */
    }
	
    // the parent will respond to the UDP connections
    else {
    		 int childpid_date, index, i;
    		 char date_buf[256];
    		 char fromEcho_c[256];
    		 char toLog_s[1024];
	 // create an infinite loop for continuity
         while (1) {
	     // receive the input from the client and output an error if it fails
   		 bzero(fromEcho_c,256);
		 for(port = 0; port < noport; port++){
             n = recvfrom(sock[port],fromEcho_c,256,0,(struct sockaddr *)&from,&fromlen);
             if (n < 0) error("ERROR receiving from");
		 
	     // fork a child and output an error if it fails
             pid = fork();
             if (pid < 0)
                 error("ERROR on fork");
		 
	     // the child will respond to the client and act accordingly
             if (pid == 0)  {
	     // the child will respond to the client and act accordingly
	     // the child will respond to the client and act accordingly
             if (pid == 0)  {

		 // create a pipe for executing the pipe command
    		 int date_pipe[2];
    		 pipe(date_pipe);

		 // fork a child for the date execution and output an error if it fails
	     // the child will respond to the client and act accordingly
             if (pid == 0)  {

		 // create a pipe for executing the pipe command
    		 int date_pipe[2];
    		 pipe(date_pipe);

		 // fork a child for the date execution and output an error if it fails
   		 if((childpid_date = fork()) == -1) error("ERROR on fork");
   		 
		 // the child will perform the date execution
		 if(childpid_date == 0)
   		 {
	 	     // replace the stdout with date_pipe
       		     dup2(date_pipe[1], 1);
			 
		     // close all the pipes
       		     close(date_pipe[0]);
		     close(date_pipe[1]);
		
		     // execute 'date +"%F %T"'
       		     char *date_args[] = {"date", "+\"%F %T\"", NULL};
       		     execvp(*date_args, date_args);
			 
		     // exit the child process
       		     exit(0);
   		 }
		     
		 // read from the date_pipe and place the contents into the date_buf; output an error if it fails
   		 n = read(date_pipe[0], date_buf, sizeof(date_buf));
   		 if(n < 0) error("ERROR reading from socket");
		     
		 // close all the pipes
   		 close(date_pipe[0]); close(date_pipe[1]);
           
           // display the message sent from the client to the stdout
           printf("Here is the message: %s\n",fromEcho_c);

	   // reply to the client, output an error if it fails
	   n = sendto(sock[port],fromEcho_c,17, 0,(struct sockaddr *)&from,fromlen);
             if (n < 0) error("ERROR sending to");
   	   

   	   // toLog_s = "timeAndDate"
   	   for(index = 0; date_buf[index] != '\0' && index < strlen(date_buf); index++)
	       toLog_s[index] = date_buf[index];
		     
   	   // toLog_s += "	"
   	   toLog_s[index] = '\t';
		     
   	   // toLog_s += "messageFromClient"
   	   for(i = 0, index=index+1; fromEcho_c[i] != '\0' && i < strlen(fromEcho_c); i++, index++)
	       toLog_s[index] = fromEcho_c[i];
		     
   	   // toLog_s += ""
   	   toLog_s[index] = '\0';
		     
   	         // send toLog_s to the log server
   	         n = sendto(sockfd_log, toLog_s, strlen(toLog_s), 0, (struct sockaddr *)&from, fromlen);
		 if (n < 0)
                     error("ERROR writing to socket LOG");
	     }
   	         // exit the child process
   	         exit(0);
	     }
	 }
    }
	
    // close all sockets
    close(sockfd);
    close(sock);
    close(newsockfd);

    return 0;
}
