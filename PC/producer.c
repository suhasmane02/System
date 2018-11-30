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

void parse_file_and_share(char *filename, struct shared_object *pso, int buffer_size);
void usage();

// ********************* parse input file and process ******************** /
void parse_file_and_share(char *filename, struct shared_object *pso, int buffer_size)
{
	struct shared_buffer *sb = (struct shared_buffer *) pso->object;

	char *var1[] = {"helllllllllllllllllllo", "hiiiiiiiiiiiiiiiiiiiii", "byyyyyyyyyrrrrrreeeee", "qwertyuiopasdfghjklzx"};

	printf("inside %s:%s\n", __FILE__, __func__);
	dump_so(pso);

	int count = sb->count = pso->object_count;
	struct buffer *b = &(sb->buffers[0]);
	for(int i = 0; i < count; i++)
	{
		char *ptr = (char *) b;
		ptr = ptr + sizeof(struct buffer) + buffer_size;
		pthread_mutex_lock(&(b->pmutex));
		strncpy(b->buff, var1[i], strlen(var1[i]));
		pthread_mutex_unlock(&(b->pmutex));
		b = (struct buffer *) ptr;
	}

	return;
}

// ********************* usage ******************** /
void usage()
{
	printf("-----USAGE-----\n");
	printf("producer <number_of_buffers> <sizeof_buffer> <file_to_process>\n");
}

int main(int argc, char *argv[])
{
	if(argc < 4)
	{
		usage();
		return -1;
	}
	int count = atoi(argv[1]);
	int size = atoi(argv[2]);
	char *filename = argv[3];

	struct shared_object *pso = create_and_init_shared_buffer(size, count, 1);
	printf("inside %s:%s\n", __FILE__, __func__);
	dump_so(pso);

	parse_file_and_share(filename, pso, 32);

	free(pso);
	return 0;
}
