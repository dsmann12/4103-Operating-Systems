/* 
6. Using sockets to write a backdoor using the client/server model.
     [32] Write a server which accepts connections from a client.  The server
     should accept commands in the form of string data from the client, execute
     them locally, then send output back to the client.  The client should
     support sending commands typed via stdin locally.  Call the server-side
     program ``pony.c`` and the client-side ``carrot.c``.  For the purpose
     of the assignment, your client/server should both operate on localhost
     (though this could easily be changed to a host of your choosing).
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include "wrappers.h"

#define BUFSIZE 4096

int main(int argc, char *argv[]) {
	struct addrinfo hints;
	struct addrinfo *result, *rptr;
	int sockfd, servfd, s;
	char buf[BUFSIZE], output[BUFSIZE];

	if (argc < 3) {
		usage(argv[0], "HOST PORT");
	}
	
	// zero hints struct
	memset(&hints, 0, sizeof(struct addrinfo));

	// Set hints data
	hints.ai_family = AF_UNSPEC;				// Allow IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;			// TCP Socket
	hints.ai_flags = AI_ADDRCONFIG;				// TCP Flag
	hints.ai_protocol = 0;						// Any protocol

	// Get list of address structures
	if ((s = getaddrinfo(argv[1], argv[2], &hints, &result)) != 0) {
		error_handler("Error getting address structures");
	}
	
	// Traverse list of address structures until successful connection
	for (rptr = result; rptr != NULL; rptr = rptr->ai_next) {
		// get socket with rptr members
		if ((sockfd = socket(rptr->ai_family, rptr->ai_socktype, rptr->ai_protocol)) < 0) {
			continue;
		}

		if (connect(sockfd, rptr->ai_addr, rptr->ai_addrlen) != -1) {
			break;								// Success
		}

		Close(sockfd);
	}	
	
	// No address succeeded
	if (rptr == NULL) {
		error_handler("Could not connect to any address");
	}

	// Clean address info
	freeaddrinfo(result);

	// Handle connection
	printf("Please enter the message: ");

	if ((fgets(buf, BUFSIZE, stdin)) != NULL) {
		if (send(sockfd, buf, BUFSIZE, 0) < 0) {
			error_handler("Error: Error writing to socket");
		}

		while (recv(sockfd, output, BUFSIZE, 0) > 0) {
			printf("%s", output);
		}

		memset(buf, 0, BUFSIZE);
		memset(output, 0, BUFSIZE);
	}

	Close(sockfd);

	return 0;
}
