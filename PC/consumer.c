#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include "shared_object.h"
#include "shared_buffer.h"

void consume(struct shared_object *pso);
void usage();

// ********************* parse input file and process ******************** /
void consume(struct shared_object *pso)
{
	struct shared_buffer *sb = (struct shared_buffer *) pso->object;
	int count = sb->count;

	struct buffer *b = &(sb->buffers[0]);
	int size = sb->size_of_each;
	char *pqr;
	pqr = malloc(size + 1);
	for(int i = 0; i < count; i++)
	{
		char *ptr = (char *) b;
		pthread_mutex_lock(&(b->pmutex));
		printf("struct buffer addr = %p\n", ptr);
		ptr = ptr + sizeof(struct buffer) + size;
		bzero(pqr, size+1);
		strncpy(pqr, b->buff, size);
		pthread_mutex_unlock(&(b->pmutex));
		b = (struct buffer *) ptr;
		printf("val %d is : %s\n", i, pqr);
	}
	free(pqr);

	return;
}

// ********************* usage ******************** /
void usage()
{
	printf("-----USAGE-----\n");
	printf("consumer <number_of_buffers> <sizeof_buffer>\n");
}

int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		usage();
		return -1;
	}
	int count = atoi(argv[1]);
	int size = atoi(argv[2]);

	struct shared_object *pso = create_and_init_shared_buffer(size, count, 0);

	consume(pso);
	free(pso);

	return 0;
}
