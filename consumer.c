#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <semaphore.h>

#define SHARED_MEM "/shared_mem_test"
#define SEMD "/sem_data"
#define SEMB "/sem_buffer"
#define BUFFER_SIZE 1024

struct shared_memory {
	char buffer [BUFFER_SIZE];
	unsigned buffer_length;
};

int main(){
	struct shared_memory *shb_ptr;
	char local_buf[BUFFER_SIZE];
	sem_t *mtxD, *mtxB;

	mtxD = sem_open(SEMD, 0, 0, 0);
	mtxB = sem_open(SEMB, 0, 0, 0);

	int fs = shm_open(SHARED_MEM, O_RDONLY, 0);
	ftruncate(fs, sizeof(struct shared_memory));
	if((shb_ptr = mmap(0, sizeof(struct shared_memory), PROT_READ, MAP_SHARED, fs, 0)) == MAP_FAILED)
		fprintf(stderr, "failed mapping\n");

	sem_post(mtxB);

	while(1) {
		sem_wait(mtxD);
		strcpy(local_buf, shb_ptr->buffer);
		printf("%s transmitted %d bytes\n", local_buf, shb_ptr->buffer_length);
		sem_post(mtxB);
	}
	printf("\n");
	munmap(shb_ptr, sizeof(struct shared_memory));
	//shm_unlink(SHARED_MEM);
	return 0;
}
