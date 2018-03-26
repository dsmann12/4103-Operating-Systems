/*
3. Utilizing pipes to calculate combinations.
     [16] Write a program which can calculate *(m choose n)*, which is a count
     of the number of possible combinations of *n*-number items from a pool of
     *m*-number items. For example, the number of five-card hands possible from
     a standard 52-card deck is (52 choose 5), which is  ``(52 x 51 x 50 x 49 x
     48) / (1 x 2 x 3 x 4 x 5)``. 
      
     The program should accept m and n as command-line arguments. It should
     then fork.  The parent should calculate *m!/((m-n)!n!)*, which is a
     product of *n* numbers.  The child should calculate *n!*.  A pipe will
     then be used to transfer the result of *n!* to the parent, which will then
     complete the calculation by dividing the two results, and output the final
     result to stdout.  Call this ``choose.c``.

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "wrappers.h"

#define READ 0
#define WRITE 1

long double fact(const unsigned int m, const unsigned int n) {
	long double p = 1;
	for (int i = (m - n) + 1; i <= m; i++) {
		p *= i;
	}

	return p;
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		usage(argv[0], "M N");
	}

	// get m and n
	// child result and parent result
	// total result
	unsigned int m = atoi(argv[1]), n = atoi(argv[2]);
	long double parent_result, child_result, total_result;
	parent_result = child_result = total_result = 1;

	int pipefd[2];

	if (n == 0) {
		error_handler("Error: N cannot equal 0");
	}

	// create pipe
	if (pipe(pipefd) == -1) {
		error_handler("Error: Error creating pipe");
	}	
	
	// fork
	pid_t pid = fork();
	
	// check fork error
	if (pid < 0) {
		error_handler("Error: Error forking");
	} else if (pid == 0) {
		// child process
		Close(pipefd[READ]);

		child_result = fact(n, n);
		
		printf("Child result is %.2Lf\n", child_result);	
	
		if ((write(pipefd[WRITE], &child_result, sizeof(child_result))) < 0) {
			error_handler("Error: Error writing to pipe");
		}

		Close(pipefd[WRITE]);
	} else {
		// parent process
		Close(pipefd[WRITE]);
		parent_result = fact(m, n);
		printf("Parent result is %.2Lf\n", parent_result);
		
		if ((read(pipefd[READ], &child_result, sizeof(child_result))) < 0) {
			error_handler("Error: Error reading from pipe");
		}

		printf("Got child result: %.2Lf\n", child_result);
		Close(pipefd[READ]);
		
		total_result = parent_result / child_result;
		printf("Total result: %.2Lf\n", total_result);
	}

	return 0;

}
