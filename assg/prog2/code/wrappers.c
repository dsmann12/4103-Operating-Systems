#include "wrappers.h"

#define MAXUSAGESIZE 1024

// Print error to STDERR and exit process
void error_handler(const char *msg) {
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}

void usage(const char *program, const char *args) {
	char msg[MAXUSAGESIZE];
	snprintf(msg, MAXUSAGESIZE, "Usage: %s %s", program, args);
	error_handler(msg);
}

void Close(const int fd) {
	if ((close(fd)) < 0) {
		error_handler("Error: Error closing file descriptor");
	}
}
