/*
1. Echo input using system calls.
     [16] Write a program which accepts a filename as a command-line argument,
     then continuously reads a line from stdin and outputs the same line to
     stdout and to the file (as indicated by the filename).  You must use only
     system calls (no calls to ``printf`` or ``scanf`` are allowed).  You can
     ensure that you use only system calls by verifying the manual page section
     of the corresponding libc API function (e.g. ``man 2 open`` yields a
     result, so ``open`` is a system call). Call this ``echo.c``. Be sure to
     distinguish this echo from the system command ``echo``.
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "wrappers.h"

#define MAXLINE 4096

int main(int argc, char *argv[]) {
	int fd = -1, nchar = 0;
	char buffer[MAXLINE];

	// check that there is at least one argument for filename
	if (argc < 2) {
		usage(argv[0], "FILENAME");
	}
	
	// open file for writing only, create if necessary
	// append to file if file already exists
	if ((fd = open(argv[1], O_WRONLY | O_CREAT | O_APPEND, 
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
		error_handler("Error opening file");
	}
	
	// read from stdin
	while ((nchar = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
		// write to stdout
		if (write(STDOUT_FILENO, buffer, nchar) < 0) {
			error_handler("Error: Error writing to stdout");
		}

 		// write to file
		if (write(fd, buffer, nchar) < 0) {
			error_handler("Error writing to file");
		}
	}
	
	return 0;
}
