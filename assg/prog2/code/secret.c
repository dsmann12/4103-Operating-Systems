#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "wrappers.h"

#define SIZE 4096

int main(int argc, char *argv[]) {
	if (argc < 2) {
		const char *name = "dscheu1";
		const char *message = "This is David's secret message!";
		int shm_write_fd = shm_open(name, O_CREAT | O_RDWR, 00644);

		if (shm_write_fd < 0) {
			error_handler("Error openeing shared memory object");
		}

		if ((ftruncate(shm_write_fd, SIZE)) < 0) {
			error_handler("Error truncating shared memory object");
		}

		void *shm_write_ptr = mmap(NULL, SIZE, PROT_WRITE, MAP_SHARED, shm_write_fd, 0);
		if (shm_write_ptr == (void *) -1) {
			error_handler("Error mapping memory");
		}
		snprintf(shm_write_ptr, SIZE, "%s", message);
	} else {
		const char *shm_name = argv[1];
		int shm_read_fd = shm_open(shm_name, O_RDONLY, 00644);
		if (shm_read_fd < 0) {
			error_handler("Error opening shared memory object");
		}
		void *shm_read_ptr = mmap(NULL, SIZE, PROT_READ, MAP_SHARED, shm_read_fd, 0);
		if (shm_read_ptr == (void *) -1) {
			error_handler("Error mapping memory");
		}	
		printf("Message: %s\n", (char *) shm_read_ptr);
	}

	return 0;
}
