/*
5. Systems calls used to create a self-replicating program.
     [16] More affectionately known as a *virus*; however this assignment
     only involves executables which self-replicate locally.
     
     Create a program which copies itself (the running executable) into another
     file with a random name of 8 lowercase characters.  It should then make a
     system call to ``chmod`` to modify the permissions of the copy so as to
     make it executable.  Finally it should execute it using one of the
     ``exec`` family of functions, thus propagating its replication. Name this
     ``kitties.c``. 

     You must develop this on your OWN computer, and ideally include a
     mechanism for stopping it after e.g. the 10th replication. **If you *dare*
     execute such a program on my server, you will automatically fail the
     second test.**

     Local virii are relatively harmless, unless they do some secondary task
     such as deleting files/directories.  At most, they consume all the space
     on the filesystem, thus preventing any files from being created and making
     the system temporarily unusable.  To recover, the filesystem must be
     externally mounted and the files cleaned, then the system must be
     rebooted.  If you are using a Live Linux medium you should just be able to
     reboot to recover (though you will lose the file you were working on);
     otherwise see me if you need help recovering.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "wrappers.h"

#define FILENAMESZ 11
#define BUFSIZE 4096

char * generate_filename() {
	char *str = NULL;
	struct timespec spec;
	long t;

	// allocate memory for string
	if ((str = (char *) malloc(FILENAMESZ)) == NULL) {
		error_handler("Error: Error creating filename string");
	}
	
	// populate spec struct
	clock_gettime(CLOCK_REALTIME, &spec);
	t = spec.tv_nsec;

	// seed RNG by time in nanoseconds
	srand(t);

	// Set first and second char to make file in current directory
	str[0] = '.';
	str[1] = '/';
	
	// Zero str buffer
	for(int i = 2; i < FILENAMESZ-1; i++) {
		str[i] = 'a' + (rand() % 26);
	}

	// null byte
	str[FILENAMESZ-1] = 0;
	
	return str;
}

int main(int argc, char *argv[]) {
	int fd, newfd;
	int decrement = 0;	
	char *filename = NULL;
	char buf[BUFSIZE];	
	pid_t pid = -1;

	// check for argument
	if (argc < 2) {
		usage(argv[0], "LIMIT");
	}

	// safety check
	if ((decrement = atoi(argv[1])) <= 0) {
		return 0;
	}


	// generate filename of 8 random lowercase characters
	filename = generate_filename();
	
	// open this program file
	if ((fd = open(argv[0], O_RDONLY)) < 0) {
		error_handler("Error: Error opening file");
	}

	// open new filename
	if ((newfd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 
					S_IRWXU | S_IRWXG | S_IRWXO)) < 0) {
		error_handler("Error: Error opening new file");
	}
	
	// read from this program file
	// write to new filename
	int nchar = 0;
	while((nchar = read(fd, buf, BUFSIZE)) > 0) {
		if (write(newfd, buf, nchar) < 0) {
			error_handler("Error: Error writing to new file");
		}
	}

	// chmod to make new file executable
	fchmod(newfd, S_IRWXU | S_IRWXG | S_IRWXO);

	// Close file descriptors
	Close(fd);
	Close(newfd);
	
	// fork
	// call exec on new file in child process
	pid = fork();
	if (pid < 0) {
		error_handler("Error: Error forking");
	} else if (pid == 0) {
		// store decremented limit as string
		char dec[10];
		memset(dec, 0, sizeof(dec));
		snprintf(dec, 10, "%d", decrement-1);

		// set args for exec
		char *args[] = {filename, dec, NULL};	

		// call exec	
		execvp(args[0], args);
	}
	
	//free filename
	free(filename);
	wait(NULL);

	return 0;
}
