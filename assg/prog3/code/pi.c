/*
4. Approximating pi.
     [16] Write a program using pthreads which approximates pi using the Monte
     Carlo technique.  In this technique, a unit circle (of area pi) is
     circumscribed inside a square (of area 4).  Random numbers are generated
     within the square.  The number which fall inside the circle (c) is
     counted, and divided by the total generated (t).  For large t, c/t
     approximates pi/4.  
     
     Write a multi-threaded program (using the C pthread library) which creates
     four threads to independently approximate pi using this technique; then
     average the results.  Use t=10000 for each thread.  Call this ``pi.c``.
     Note that you will need to link against the pthreads library when
     compiling.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define THREADS 4
#define LIMIT 10000

void *monte_carlo(void *result) {
	int circle_points = 0, total_points = 0;
	double *res = result;
	
	// better randomness
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
	
	srand(spec.tv_nsec);
	
	for(int i = 0; i < LIMIT; i++) {
		// generate points 
		double x = ((double) (rand() % LIMIT+1)) / LIMIT;
		double y = ((double) (rand() % LIMIT+1)) / LIMIT;
	
		// distance from origin (no need to sqrt since unit circle
		double dist = x*x + y*y;
	
		if (dist <= 1)
			circle_points++;
		
		total_points++;
	}

	*res = (4.0 * circle_points) / total_points;
}

int main(void) {
	// Array to hold results from threads
	double results[THREADS];	
	double sum = 0.0, avg = 0.0;

	// Separate tid for each thread
	pthread_t tid[THREADS];

	// Give threads default attributes
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	// Create the threads and run function
	int i = 0;
	for(i = 0; i < THREADS; i++) {
		pthread_create(&tid[i], &attr, monte_carlo, &results[i]);
	}

	// wait for threads to finish and join them
	for(i = 0; i < THREADS; i++) {
		pthread_join(tid[i], NULL);
		sum += results[i];
		printf("Result from results[%d] is: %.2f\n", i, results[i]);
	}

	avg = sum / THREADS;

	printf("Average of results is: %.2f\n", avg);

	return 0;
}
