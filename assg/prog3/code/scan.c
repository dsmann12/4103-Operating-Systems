/* 
3. A port scanner.
     [16] Write a sockets program which accepts an IP address as a command-line
     argument, then scans ports 21, 22, 23, 25, 80, 143, 443, and 993 to see if
     they accept connections.  On a successful connection, close the connection
     then print out the port.  Call this ``scan.c``.  You will need to set an
     options for connection timeout to make the port scanner feasible to use
     on remote hosts. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include "wrappers.h"

#define BUFSIZE 4096
#define TIMEOUT 5

int main(int argc, char *argv[]) {
	struct addrinfo hints;
	struct addrinfo *result, *rptr;
	int sockfd, servfd, s;
	char buf[BUFSIZE], output[BUFSIZE];
	int ports[] = {21, 22, 23, 25, 80, 143, 443, 993};	

	if (argc < 2) {
		usage(argv[0], "HOST");
	}
	

	// zero hints struct
	memset(&hints, 0, sizeof(struct addrinfo));

	// Set hints data
	hints.ai_family = AF_UNSPEC;				// Allow IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;			// TCP Socket
	hints.ai_flags = AI_ADDRCONFIG;				// TCP Flag
	hints.ai_protocol = 0;						// Any protocol

	for(int i = 0; i < 8; i++) {
		char portno[3];
		snprintf(portno, sizeof(portno), "%d", ports[i]);

		//set start time
		time_t start_time = time(NULL);
		if (start_time == ((time_t) -1)) {
			error_handler("Error: Failure to obtain current time");
		}
		
		// Get list of address structures
		if ((s = getaddrinfo(argv[1], portno, &hints, &result)) != 0) {
			error_handler("Error getting address structures");
		}
	
		// Traverse list of address structures until successful connection
		for (rptr = result; rptr != NULL; rptr = rptr->ai_next) {
			//current time of loop
			time_t current_time = time(NULL);
			if (current_time == ((time_t) -1)) {
				error_handler("Error: Failure to obtain current time");
			}
	

			printf("difftime is %f\n", difftime(current_time, start_time));
			if((difftime(current_time, start_time)) >= 2.0){
				rptr = NULL;
				break;
			}

			printf("Before socket\n");
			// get socket with rptr members
			if ((sockfd = socket(rptr->ai_family, rptr->ai_socktype | SOCK_NONBLOCK, rptr->ai_protocol)) < 0) {
				continue;
			}
			printf("After socket\n");
		
			printf("Before connect\n");
			if (connect(sockfd, rptr->ai_addr, rptr->ai_addrlen) != -1) {
				printf("After connect succeeded\n");
				break;								// Success
			}
			printf("After connect failed\n");

			Close(sockfd);
		}	
		
		printf("Outside for loop\n");	
		// No address succeeded
		if (rptr != NULL) {
			printf("[+]Connected: Port: %d\n", ports[i]);
			Close(sockfd);
		} else {
			printf("[-] Not connected: Port: %d\n", ports[i]);
		}

		// Clean address info
		freeaddrinfo(result);
	}

	return 0;
}
