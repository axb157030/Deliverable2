// echo_c.c
// the client of the echo server that sends a message and its ip address to the echo server
//   through TCP or UDP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "echo_c_functions.h"

int main(int argc, char *argv[])
{
    // determine whether the user wants a TCP or UDP connection
    int ans_str;
    printf("UDP(1) or TCP(2): ");
    scanf("%d", &ans_str);
    getchar();

    // if the user wants a TCP connection
    if(ans_str == 2)
    {
    	int sockfd, portno, n;
    	struct sockaddr_in serv_addr;
    	struct hostent *server;
    	int childpid, index, i;
    	char ip_buf[256];
    	char message_buf[256];
    	char buffer[256];
    	char toEcho_s[1024];
    	char wasRecvdFrom[] = " was received from ";
	    
	// create a pipe for executing the command to receive the ip address
    	int ip_pipe[2];
	pipe(ip_pipe);

	// output an error if no port is provided
    	if (argc < 3) {
       	    fprintf(stderr,"usage %s hostname port\n", argv[0]);
       	    exit(0);
        }
	    
	// make a TCP socket to connect to the echo server
	sockfd = makeSocket(SOCK_STREAM);
	    
	// connect to the server
    	portno = atoi(argv[2]);
    	server = gethostbyname(argv[1]);
    	if (server == NULL) {
            fprintf(stderr,"ERROR, no such host\n");
            exit(0);
    	}
    	bzero((char *) &serv_addr, sizeof(serv_addr));
	    serv_addr.sin_family = AF_INET;
    	bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    	serv_addr.sin_port = htons(portno);
    	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
            error("ERROR connecting");
	
	// fork a child process, output an error if it fails
    	if((childpid = fork()) == -1)
       	    error("ERROR creating child from echo_c");
	    
	// the child process executes the hostname command
    	if(childpid == 0)
     	{
	    // replace the stdout with the pipe
	    dup2(ip_pipe[1], 1);
		
	    // close all pipes
	    close(ip_pipe[0]); close(ip_pipe[1]);
		
	    // execute 'hostname -i'
	    char *hostname_args[] = {"hostname", "-i", NULL};
	    execvp(*hostname_args, hostname_args);
		
	    // exit the child process
	    exit(0);
    	}
	// read the output of the 'hostname -i' command from the pipe, output an error if it fails
    	n = read(ip_pipe[0], ip_buf, sizeof(ip_buf));
    	if (n < 0)
            error("ERROR reading from echo_c");
	    
	// close all pipes
    	close(ip_pipe[0]); close(ip_pipe[1]);

	// receive the message from the user
    	printf("Please enter the message: ");
    	bzero(message_buf,256);
    	fgets(message_buf,255,stdin);

	// toEcho_s = "messageFromUser"
    	for(index = 0; message_buf[index] != '\0' && index < strlen(message_buf) && message_buf[index] != '\n'; index++)
            toEcho_s[index] = message_buf[index];

	// toEcho_s += " was received from "
    	for(i = 0; wasRecvdFrom[i] != '\0' && i < strlen(wasRecvdFrom); i++, index++)
            toEcho_s[index] = wasRecvdFrom[i];
    
	// toEcho_s += "ipAddress"
    	for(i = 0; ip_buf[i] != '\0' && i < strlen(ip_buf); i++, index++)
            toEcho_s[index] = ip_buf[i];
	    
	// toEcho_s += ""
    	toEcho_s[index] = '\0';

	// write toEcho_s to echo server and output an error if it fails
    	n = write(sockfd,toEcho_s,strlen(toEcho_s));
    	if (n < 0) 
            error("ERROR writing to socket");
	
	// read from echo server and output an error if it fails
    	bzero(buffer,256);
    	n = read(sockfd,buffer,255);
    	if (n < 0) 
            error("ERROR reading from socket");
	    
	// output the message from echo server to the stdout
    	printf("%s\n",buffer);
	    
	// close the socket
    	close(sockfd);
	    
	// terminate the program
    	return 0;
    }
	
    // if the user wants a UDP connection
    else if(ans_str == 1)
    {
    	int sock, n, childpid, index, i;
    	unsigned int length;
    	char ip_buf[256];
    	char message_buf[256];
    	char buffer[256];
    	char toEcho_s[1024];
 	struct sockaddr_in server, from;
    	struct hostent *hp;
   
	// create a pipe for executing the command to receive the ip address
    	int ip_pipe[2];
	pipe(ip_pipe);
	
	// output an error if no port is provided
  	if (argc != 3) {
	    printf("Usage: server port\n");
            exit(1);
    	}
	    
	// make a UDP socket to connect to the echo server
    	sock= makeSocket(SOCK_DGRAM);

	// store server information and get hostname
    	server.sin_family = AF_INET;
    	hp = gethostbyname(argv[1]);
    	if (hp==0) error("Unknown host");
    	bcopy((char *)hp->h_addr, 
          (char *)&server.sin_addr,
          hp->h_length);
    	server.sin_port = htons(atoi(argv[2]));
    	length=sizeof(struct sockaddr_in);
	
	// fork a child process, output an error if it fails
    	if((childpid = fork()) == -1)
       	    error("ERROR creating child from echo_c");
	    
	// the child process executes the hostname command
    	if(childpid == 0)
     	{
	    // replace the stdout with the pipe
	    dup2(ip_pipe[1], 1);
		
	    // close all pipes
	    close(ip_pipe[0]); close(ip_pipe[1]);
		
	    // execute 'hostname -i'
	    char *hostname_args[] = {"hostname", "-i", NULL};
	    execvp(*hostname_args, hostname_args);
		
	    // exit the child process
	    exit(0);
    	}
	// read the output of the 'hostname -i' command from the pipe, output an error if it fails
    	n = read(ip_pipe[0], ip_buf, sizeof(ip_buf));
    	if (n < 0)
            error("ERROR reading from echo_c");
	    
	// close all pipes
    	close(ip_pipe[0]); close(ip_pipe[1]);   
	 
	// receive the message from the user
    	printf("Please enter the message: ");
    	bzero(buffer,256);
    	fgets(buffer,255,stdin);
	
	// toEcho_s = "messageFromUser"
    	for(index = 0; message_buf[index] != '\0' && index < strlen(message_buf) && message_buf[index] != '\n'; index++)
            toEcho_s[index] = message_buf[index];

	// toEcho_s += " was received from "
    	for(i = 0; wasRecvdFrom[i] != '\0' && i < strlen(wasRecvdFrom); i++, index++)
            toEcho_s[index] = wasRecvdFrom[i];
    
	// toEcho_s += "ipAddress"
    	for(i = 0; ip_buf[i] != '\0' && i < strlen(ip_buf); i++, index++)
            toEcho_s[index] = ip_buf[i];
	    
	// toEcho_s += ""
    	toEcho_s[index] = '\0';
	    
	// write toEcho_s to echo server and output an error if it fails
    	n=sendto(sock,toEcho_s,
            strlen(toEcho_s),0,(const struct sockaddr *)&server,length);
    	if (n < 0) error("Sendto");
	    
	// read from echo server and output an error if it fails
    	n = recvfrom(sock,buffer,256,0,(struct sockaddr *)&from, &length);
    	if (n < 0) error("recvfrom");
	    
	// output the message from echo server to the stdout
    	write(1,"Got an ack: ",12);
    	write(1,buffer,n);
	    
	// close the socket
    	close(sock);
	 
	// terminate the program
    	return 0;
    }
	
    // if the user doesn't respond with UDP or TCP
    else
        error("ERROR: Must answer '1' or '2'");
	
    // terminate the program
    return 0;
}
