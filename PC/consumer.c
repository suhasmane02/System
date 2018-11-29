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

int init_mutex(void *pmutexptr);
int destroy_mutex(void *pmutexptr);
int init_shared_buffer(void *object);
int destroy_shared_buffer(void *object);
struct shared_object* create_and_init_shared_buffer(int size, int count);
void consume(struct shared_object *pso);
void usage();

// ******************** SHARED BUFFER ********************* /
struct buffer {
	pthread_mutex_t pmutex;
	char buff[0];
};

struct shared_buffer {
	int current_in_use;
	int size_of_each;
	int count;
	struct buffer buffers[0];
};

// ********************* mutex callbacks ******************** /
int init_mutex(void *pmutexptr)
{
	pthread_mutex_t *pmutex = (pthread_mutex_t *) pmutexptr;
	pthread_mutexattr_t attrmutex;

	pthread_mutexattr_init(&attrmutex);
	pthread_mutexattr_setpshared(&attrmutex, PTHREAD_PROCESS_SHARED);

	pthread_mutex_init(pmutex, &attrmutex);
	return SUCCESS;
}

int destroy_mutex(void *pmutexptr)
{
	pthread_mutex_t *pmutex = (pthread_mutex_t *) pmutexptr;
	pthread_mutex_destroy(pmutex);
	return SUCCESS;
}

// ********************* shared buffer callbacks ******************** /
int init_shared_buffer(void *object)
{
	struct shared_object *so = (struct shared_object *) object;
	int count = so->object_count;
	struct shared_buffer *sb = (struct shared_buffer *) so->object;
	sb->size_of_each = count;
	sb->current_in_use = -1;
	sb->size_of_each = (so->object_size - sizeof(struct shared_buffer))/count;

	for(int i = 0; i < count; i++)
	{
		init_mutex(&(sb->buffers[i].pmutex));
	}
	return SUCCESS;
}

int destroy_shared_buffer(void *object)
{
	struct shared_object *so = (struct shared_object *) object;
	struct shared_buffer *sb = (struct shared_buffer *) so->object;

	int count = sb->count;

	for(int i = 0; i < count; i++)
	{
		destroy_mutex(&(sb->buffers[i].pmutex));
	}
	return SUCCESS;
}

// ********************* shared buffer ******************** /
struct shared_object* create_and_init_shared_buffer(int size, int count)
{
	struct shared_object *so;
	char buf_name[10] = {0};

	so = malloc(sizeof(struct shared_object));
	sprintf(buf_name, "BUFFER");
	strcpy(so->object_name, buf_name);
	so->object_count = count;
	so->object_size = sizeof(struct shared_buffer) + (sizeof(struct buffer)+size) * count;

	so->init = (void *) 0;
	so->cleanup = destroy_shared_buffer;

	return create_shared_object(so);
}

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
	printf("producer <number_of_buffers> <sizeof_buffer>\n");
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

	struct shared_object *pso = create_and_init_shared_buffer(size, count);

	consume(pso);
	free(pso);

	return 0;
}
