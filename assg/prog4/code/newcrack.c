/*
2. Brute-force password decryption using multi-threading.
     [32] Encryption algorithms work by operating on a password to produce a
     hash.  Brute-force password cracking works by applying an encryption
     algorithm to a range of possible passwords, and checking if each of the
     resulting hashes turned up by the encryption algorithm is equal to an
     acquired hash. 

     Naturally, this takes a long time. Anything which is known about the
     password helps a brute-force attempt--such as knowing that the password
     has only lowercase characters, or that it is exactly eight characters in
     length, and so forth.

     A file called ``crack.c`` is provided which contains a serial decryption
     algorithm for use with Linux ``crypt(3)``, and a file called ``hash``
     which contains a password hash for the password ``abba``.  Run it and
     observe how it works.  It assumes the password is 4 lowercase characters
     and that the salt is also lowercase characters.  
     
     Write a multi-threaded version using pthread to run the serial algorithm
     on all possible four-character lowercase-letter passwords, and check to
     see if the hashes are equal.  If they are, output the password to stdout
     and terminate the program.  Use four threads.  Call it ``crack.c``, as the
     original is called.
     
     You will need to be careful with pointers; in particular, ``crypt`` uses
     only one pointer to reference its result, so you will need to avoid race
     conditions in the multi-threaded version.  Use a mutex lock during calls
     to ``crypt`` and data copies from the pointer returned by crypt.
*/
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <crypt.h>
#include <pthread.h>

#define THREADS 4
#define PWSIZE 4
#define SALTSIZE 2
#define CHARSIZE 26
#define TOTALPW (int) pow(CHARSIZE, PWSIZE)
#define TOTALSALT (int) pow(CHARSIZE, SALTSIZE)
#define PORTION TOTALPW/THREADS

pthread_mutex_t mutex;

struct thread_data {
	unsigned int start_index;
	unsigned int end_bound;
	const char *target;	
};

struct thread_data *thread_data_constructor(unsigned int si, unsigned int eb, const char *target) {
	struct thread_data *data = (struct thread_data *) malloc(sizeof(struct thread_data));
	data->start_index = si;
	data->end_bound = eb;
	data->target = target;
	
	return data;
}

void ith(char *password, int index, int n) {
	for (unsigned int i = 1; i <=n; i++) {
		password[i-1] = (index % 26) + 'a';
		index /= 26;
	}
	password[n] = '\0';
}

// function to call for each thread
void *runner(void *tdata) {
	// bounds data and target for thread
	struct thread_data *data = tdata;
	unsigned int start_index = data->start_index;
	unsigned int end_bound = data->end_bound;
	const char *target = data->target;
	// each thread has its own password
	char password[PWSIZE+1];
	char salt[SALTSIZE+1];	

	printf("Before loop\n");	
	for(unsigned int i = start_index; i < end_bound; i++) {
		ith(password, i, PWSIZE);
		for (unsigned int j = 0; j < TOTALSALT; j++) {
			ith(salt, j, SALTSIZE);

			// lock mutex
			pthread_mutex_lock(&mutex);
			char *hash = crypt(password, salt);

			// check if hash is corret
			if ((0 == strcmp(password, "azza")) && (0 == strcmp(salt, "xy"))) {
				printf("Correct guess\n");
			}			
			if (0 == strcmp(hash, target)) { 	// should find a way to use strncmp
				printf("Password: %s\t\tSalt: %s\t\tHash: %s\t\tTarget: %s\n", password, salt, hash, target);
				printf("Found found found\n");
				exit(0);
			} else {
				printf("Trying %s...\r", password);
			}
			pthread_mutex_unlock(&mutex);
		}
	}

	free(data);
}

int main(void) {
	// Hash value in hash file
	char target[16];

	// Initialize mutex
	pthread_mutex_init(&mutex, NULL);

	// Initialize threads
	pthread_t tid[THREADS];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	
	// Read value from hash file
	int fd = open("hash", 'r');
	int num = 0;
	if ((num = read(fd, target, 16)) <= 0) {
		fprintf(stderr, "Error: Error reading file");
		exit(-1);
	} else {
		target[num-1] = 0;
	}
	close(fd);

	int i = 0;
	for (int t = 0; t < (THREADS - 1); t++) {
		unsigned int eb = i + PORTION;
		struct thread_data* data = thread_data_constructor(i, eb, target);
		printf("For thread [%d], start_index is: %d\n", t, data->start_index);
		printf("For thread [%d], end_bound is: %d\n", t, data->end_bound);
		printf("For thread [%d], target is %s\n", t, data->target);
		pthread_create(&tid[t], &attr, runner, data);
		i = eb;
	}

	struct thread_data* data = thread_data_constructor(i, TOTALPW, target);
	printf("Last thread has i: %d and end_bound is %d\n", i, TOTALPW);
	pthread_create(&tid[THREADS-1], &attr, runner, data);

	// Wait for threads
	for (i = 0; i < THREADS; i++) {
		pthread_join(tid[i], NULL);
	}
	
	printf("Could not find password. Must be error in assumptions\n");
	
	return 0;
}










