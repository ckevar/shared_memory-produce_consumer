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
	char local_buf[] = "hola shared buffer";
	sem_t *mtxD, *mtxB;

	mtxD = sem_open(SEMD, O_CREAT, 0666, 0);
	mtxB = sem_open(SEMB, O_CREAT, 0666, 0);

	int fs = shm_open(SHARED_MEM, O_RDWR | O_CREAT, 0666);
	if (fs < 0) fprintf(stderr, "failed opening\n");
	ftruncate(fs, sizeof(struct shared_memory));

	if((shb_ptr = mmap(NULL, sizeof(struct shared_memory), PROT_WRITE, MAP_SHARED, fs, 0)) == MAP_FAILED)
		fprintf(stderr, "error mapping\n");


	while(1) {
		sem_wait(mtxB);
		shb_ptr->buffer_length = strlen(local_buf);
		strcpy(shb_ptr->buffer, local_buf);
		sem_post(mtxD);
		usleep(30000);
		printf("I'm done\n");
	}
	munmap(shb_ptr, sizeof(struct shared_memory));
	shm_unlink(SHARED_MEM);
	return 0;
}
