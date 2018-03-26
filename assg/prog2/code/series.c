/*
2. Forking processes to calculate series and products.
     [16] Write a program which accepts an integer command-line argument (call
     it ``n```).  The program should then fork.  The parent process shall
     calculate the product of numbers 1 to n, while the child process
     calculates the sum of numbers 1 to n (so they may potentially be done
     concurrently on a multiprocessor system).  The parent should wait for the
     child to finish executing before printing its result.  Call this
     ``series.c``.
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "wrappers.h"

#define MAXSTRSIZE 4096

// product function
// unsigned long double instead
long double product(unsigned int n) {
	if (n == 0) {
		return 0;
	}

	//1 to n
	long double p = 1;
	for (int i = 2; i <= n; i++) {
		p *= i;
	}

	return p;
}

// sum function
unsigned long long sum(unsigned int n) {
	unsigned long long s = 0;
	for (int i = 1; i <= n; i++) {
		s += i;
	}

	return s;
}

int main(int argc, char *argv[]) {

	// check if at least one argument
	if (argc < 2) {
		usage(argv[0], "NUMBER");
	}

	// get argument as integer
	unsigned int n = (unsigned) atoi(argv[1]);

	// fork
	pid_t pid = fork();
	
	if (pid < 0) {
		// Error occurred
		error_handler("Error forking");
	} else if (pid == 0) {
		// Child process
		unsigned long long s = sum(n);
		printf("The sum from 1 to %u is %llu\n", n, s); 
	} else {
		// Parent process
		long double p = product(n);
		wait(NULL);
		printf("The product from 1 to %u is %.15LE\n", n, p);
	}

	return 0;
}
