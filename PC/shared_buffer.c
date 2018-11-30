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
	sb->count = count;
	sb->current_in_use = -1;
	sb->size_of_each = (so->object_size - sizeof(struct shared_buffer))/count - sizeof(struct buffer);

	printf("inside %s:%s\n", __FILE__, __func__);
	dump_so(so);
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
struct shared_object* create_and_init_shared_buffer(int size, int count, short is_producer)
{
	struct shared_object *so;
	char buf_name[10] = {0};

	so = (struct shared_object *) malloc(sizeof(struct shared_object));
	sprintf(buf_name, "BUFFER");
	strcpy(so->object_name, buf_name);
	so->object = (void *) 0;
	so->object_count = count;
	so->object_size = sizeof(struct shared_buffer) + (sizeof(struct buffer)+size) * count;

	if(is_producer)
		so->init = init_shared_buffer;
	else
		so->init = (void *) 0;
	so->cleanup = destroy_shared_buffer;

	struct shared_object *pso = create_shared_object(so);
	printf("inside %s:%s\n", __FILE__, __func__);
	dump_so(pso);

	return pso;
}
