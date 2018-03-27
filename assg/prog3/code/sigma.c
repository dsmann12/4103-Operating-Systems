/*
7. Sums and products.
     [16]. Using OpenMP, write a program which accepts integer start, stop, and
     step values (in that order) as command-line arguments and calculates in
     parallel both the sum and product from start to stop (inclusive) in
     increments of step.  Be sure to use the proper reductions, and check to
     be sure that the results are correct.  Call this ``sigma.c``.
*/

#include <stdio.h>
#include <stdlib.h>

long long product(int start, int stop, int step) {
	long long product = 1;
	
	#pragma omp parallel for reduction(*:product)
	for (int n = start; n <= stop; n += step) {
		product *= n;
	}

	return product;
}

long sum(int start, int stop, int step) {
	long sum = 0;
	
	#pragma omp parallel for reduction(+:sum)
	for (int n = start; n <= stop; n += step) {
		sum += n;
	}

	return sum;
}

int main(int argc, char *argv[]) {
	int start = 0, stop = 0, step = 0;
	long long p = 1;
	long s = 0;

	if (argc < 4) {
		fprintf(stderr, "Usage: %s START STOP STEP\n", argv[0]);
		return -1;
	}

	start = atoi(argv[1]);
	stop = atoi(argv[2]);
	step = atoi(argv[3]);

	p = product(start, stop, step);
	s = sum(start, stop, step);

	printf("Product from %d to %d in steps of %d is: %lld\n", start, stop, step, p);
	printf("Sum from %d to %d in steps of %d is: %ld\n", start, stop, step, s);
	
	return 0;
}
