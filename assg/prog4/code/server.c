/* 
. A multi-threaded file server.
     [32] Typical web servers are file servers: they allow multiple users to
     download files from the server concurrently.  A multi-threaded file server
     does just that; it allows for multiple clients requesting different files
     instantaneously to download them concurrently.  Such servers typically
     have limits imposed on them to handle request-based DDoS attacks, such as
     only allowing *n*-number downloads at one time, or restricting bandwidth.
     
     Write a multi-threaded sockets-based client/server system ``server.c`` and
     ``client.c`` using the pthread library.  The client program should accept
     as command-line arguments an IP address and a path to a file.  The server
     should send the requested file back to the client, which should save it in
     the current directory by its basename.  
     
     For each client request, have the server create a new thread to handle it.
     You will need to use a counting semaphore to ensure that at most four
     threads are running at one time.  If the server is busy (with four threads
     running), it should honor the next client request as soon as it is able.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include "wrappers.h"

#define BACKLOG 4
#define BUFSIZE 4096


void handle(const int sockfd) {
	// read message from client
	char input[BUFSIZE];
	char output[BUFSIZE];

	memset(input, 0, BUFSIZE);
	memset(output, 0, BUFSIZE);
	int n = 0;

	while (1) {
		if ((n = recv(sockfd, input, BUFSIZE, 0)) < 0) {
			error_handler("Error reading from client socket");
		}

		if (0 == strncmp("exit", input, 4)) {
			return;
		}

		FILE *fp = NULL;
		if ((fp = popen(input, "r")) == NULL) {
			error_handler("Error running command");
		}

		// read from command output
		// while fgets works, add to buffer
		// but then after fgets, send and 
		if((fgets(output, sizeof(output), fp)) != NULL) {
			// write to socket
			if (send(sockfd, output, BUFSIZE, 0) < 0) {
				error_handler("Error: Error writing to socket");
			}
			memset(output, 0, BUFSIZE);
		}	
		
		memset(input, 0, BUFSIZE);
		pclose(fp);
	}

	return;
}

int main(int argc, char *argv[]) {
	// socket structures
	struct addrinfo hints;
	struct addrinfo *result, *rptr;
	int sockfd, clisockfd, s, optval = 1;
	struct sockaddr_storage cli_addr;
	socklen_t cli_addr_len;
	ssize_t nread = -1;
	char buf[BUFSIZE];
	char cli_host[BUFSIZE], cli_port[BUFSIZE];
	pid_t pid = -1;

	if (argc < 2) {
		usage(argv[0], "PORT");
	}

	// Zero hints struct
	memset(&hints, 0, sizeof(struct addrinfo));
	
	// Set hints data
	hints.ai_family = AF_UNSPEC; 					// Allow IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; 				// TCP socket
	hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE;	// TCP Server
	hints.ai_protocol = 0;							// Any protocol
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	// Get list of addrinfo structurs which caontain an Internte address that can be bound
	if ((s = getaddrinfo(NULL, argv[1], &hints, &result)) != 0) {
		error_handler("Error getting address structures");
	}

	// Traverse list of address structures until successfully bound
	// If socket or bind call fails, then close socket and try next address
	for (rptr = result; rptr != NULL; rptr = rptr->ai_next) {
		// get socket fd with rp members
		if ((sockfd = socket(rptr->ai_family, rptr->ai_socktype, rptr->ai_protocol)) < 0) {
			continue;
		}

		// Eliminate address already in use error
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(int)) < 0) {
			Close(sockfd);
			continue;
		}

		// bind socket fd with address
		if ((bind(sockfd, rptr->ai_addr, rptr->ai_addrlen)) == 0) {
			break;
		}

		// robust close function if failed
		Close(sockfd);
	}

	// No address succeeded
	if (rptr == NULL) {
		error_handler("Could not bind to any address");
	}
	
	// Clean up address structures as not needed anymore
	freeaddrinfo(result);

	if (listen(sockfd, BACKLOG) < 0) {
		Close(sockfd);
		error_handler("Error: Error on listen");
	}

	// Server loop
	while(1) {
		// accept
		cli_addr_len = sizeof(struct sockaddr_storage);
		if ((clisockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_addr_len)) < 0) {
			error_handler("Error: Error accepting connection");
		}

		if (getnameinfo((struct sockaddr *) &cli_addr, cli_addr_len, cli_host, BUFSIZE, 
					cli_port, BUFSIZE, 0) != 0) {
			error_handler("Error: Error getting name info");
		}

		pid = fork();

		if (pid < 0) {
			error_handler("Error: Error Forking");
		} else if (pid == 0) {
//			Close(sockfd);
			handle(clisockfd);
			close(sockfd);
			exit(EXIT_SUCCESS);
		} else {
			Close(clisockfd);
		}
	}

	return 0;
}