/*
5. Calculating the dot product.
     [16] Write a multi-threaded program using pthreads which calculates the
     dot product of two arrays *u* and *v*. To do this, split each of the
     arrays in half, calculate the dot products of the two halves, then add the
     results together.  Create two threads to achieve this end. Assume that the
     sizes of *u* and *v* are even.  Call it ``dot.c``.  In your output, show
     the two arrays *u* and *v* and the final result of the dot product.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define THREADS 2
#define DEFAULT 4 
#define RANDLIMIT 101

struct matrices {
	int *u;
	int *v;
	unsigned int sz;
	int dot;
};

void print_array(int arr[], int sz) {
	printf("[");
	for(unsigned int i = 0; i < sz; i++) {
		if(i != sz-1)
			printf("%d, ", arr[i]);
		else
			printf("%d]", arr[i]);
	}
}

void *dot(void *args) {
	struct matrices *halves = args;
	int dot = 0;

	for(int i = 0; i < halves->sz; i++) {
		dot += (halves->u[i] * halves->v[i]);
	}

	halves->dot = dot;
}

int main(int argc, char *argv[]) {
	int sz = (argc > 1) ? atoi(argv[1]) : DEFAULT;
	if (sz < 0) {
		fprintf(stderr, "Size cannot be negative\n");
		return -1;
	}
	int u[sz], v[sz];
	unsigned int i = 0;	
	struct matrices halves[2];
	int mid = sz/2;
	pthread_t tid[THREADS];
	pthread_attr_t attr;

	srand(time(NULL));
	
	// Generate arrays
	for (i = 0; i < sz; i++) {
		u[i] = (rand() % RANDLIMIT);
		v[i] = (rand() % RANDLIMIT);
	}

	// populate structs
	for (i = 0; i < 2; i++) {
		struct matrices *s = &halves[i];
		s->sz = mid;
		s->u = &u[i*mid];
		s->v = &v[i*mid];
	}

	// Give threads default attributes
	pthread_attr_init(&attr);

	// Create threads and pass to function
	for(i = 0; i < THREADS; i++) {
		pthread_create(&tid[i], &attr, dot, &halves[i]);
	}

	int dot_product = 0;
	// wait for threads to finish and join them
	for (int i = 0; i < THREADS; i++) {
		pthread_join(tid[i], NULL);
		dot_product += halves[i].dot;
	}

	// Print statements
	printf("The dot product of ");
	print_array(u, sz);
	printf(" and ");	
	print_array(v, sz);
	printf(" is: %d\n", dot_product);
	return 0;
}
