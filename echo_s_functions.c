// echo_s_functions.c
// contains the common functions used in the echo_s.c file

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/un.h>
#include <stdio.h>
#include <sys/wait.h>

// prints an error message
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

// used to deal with zombie processes
void *SigCatcher(int n)
{
  wait3(NULL,WNOHANG,NULL);
}

// creates a socket of the appropriate type
int makeSocket(int type)
{
    // creates a new socket
    int sockfd;
    sockfd = socket(AF_INET, type, 0);
        
    // outputs an error if the socket creation failed
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    // return the socket
    return sockfd;
}
