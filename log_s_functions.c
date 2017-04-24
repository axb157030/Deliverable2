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

//Function for displaying an error message
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

//Function for making a socket
void makeSocket()
{
   //Socket Creation
   sock = socket(AF_INET, SOCK_DGRAM, 0);

	//Socket Validation
  if (sock < 0){
    error("ERROR opening socket");
	}
}

void fileWrite(){
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
}
