/*
7. Writing code to shared memory to be executed by another process.
     [32] It was discussed in class that it is in theory possible to load code
     into shared memory to be executed by another process.  If you know about
     function pointers, you can do this.  It is not a practical use of IPC,
     though it may be possible to use it in conjunction with other mechanisms
     to attack a system, such as to get a privileged process to execute 
     arbitrary code stored in shared memory.

     The following is assembly code for a function ``int inc(int n)`` which
     accepts an integer and increments it by 1:

     :: 

       0x55, 0x48, 0x89, 0xe5, 0x89, 0x7d, 0xfc,
       0x8b, 0x45, 0xfc, 0x83, 0xc0, 0x01, 0x5d, 0xc3

     Assembly hex codes sequences such as this can be obtained by writing the
     function in C, then using a command ``objdump`` with the ``-d`` flag to
     disassemble it.

     Write a producer code called ``producer.c`` which loads this code into a
     shared memory block. Name the block after the reverse of your PAWS ID
     to avoid collisions with other students.

     Then write an executor code called ``executor.c`` which opens the shared
     memory block and calls the function (you require rudimentary knowledge of
     function pointers to do this).  To test it, pass in an integer argument
     and print out the return value to prove that the code was in fact
     executed.  Be sure to unlink the shared memory object when finished.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "wrappers.h"

#define FUNCSIZE 16

int main(int argc, char *argv[]) {
	// check # of args is at least 2
	if (argc < 2) {
		usage(argv[0], "NUMBER");
	}
	
	// store arg as int
	int n = atoi(argv[1]);
	
	// set shm name as reverse of lsuid
	char *name = "1uehcsd";

	// open shared memory object
	int shmfd = shm_open(name, O_RDONLY, 00744);
	if (shmfd < 0) {
		error_handler("Error: Error opening shared memory object");
	}

	// map shared memory object to memory
	void *shmptr = mmap(NULL, FUNCSIZE, PROT_EXEC | PROT_READ, MAP_SHARED, shmfd, 0);
	if (shmptr == (void *) -1) {
		error_handler("Error: Error mapping memory");
	}

	// get shmptr as pointer to increment function
	int (*function)(int) = shmptr;

	// store return value of function in x and print
	int x = (*function)(n);
	printf("%d + 1 = %d\n", n, x);
	
	// unlink shared memoery
	if (shm_unlink(name) < 0) {
		error_handler("Error: Error unlinking shared memory object");
	}
	return 0;
}
