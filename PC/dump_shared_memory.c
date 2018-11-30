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

void dump(int start, int last, struct shared_object *pso, int buffer_size);
void usage();

// ********************* dump memory ******************** /
void dump(int start, int end, struct shared_object *pso, int buffer_size)
{
	printf("start = %d, end = %d buffer_size = %d\n", start, end, buffer_size);
	if(start == 0 && end == 0)
	{
		int total_size = pso->object_size;// * pso->object_count;
		char *ptr = (char *) pso->object;
		printf("HEX DUMP of memory start = %d last = %d\n", start, end);
		for(int i = 0; i < total_size; i++)
		{
			printf("0x%x ", ptr[i]);
			if(i && i % 7 == 0)
				printf("\n");
		}
	}

/*	struct buffer *b = &(sb->buffers[0]);
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
*/
	return;
}

// ********************* usage ******************** /
void usage()
{
	printf("-----USAGE-----\n");
	printf("dump <start record number> <end record number> <size of buffer> <total records in memory>\n");
}

int main(int argc, char *argv[])
{
	if(argc < 5)
	{
		usage();
		return -1;
	}
	int start = atoi(argv[1]);
	int end = atoi(argv[2]);
	int size = atoi(argv[3]);
	int count = atoi(argv[4]);

	struct shared_object *pso = create_and_init_shared_buffer(size, count, 0);
	printf("inside %s:%s\n", __FILE__, __func__);
	dump_so(pso);

	dump(start, end, pso, size);

	free(pso);
	return 0;
}
