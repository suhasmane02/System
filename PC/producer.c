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
void parse_file_and_share(char *filename, struct shared_object *pso, int buffer_size);
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
struct shared_object* create_and_init_shared_buffer(int size, int count)
{
	struct shared_object *so;
	char buf_name[10] = {0};

	so = (struct shared_object *) malloc(sizeof(struct shared_object));
	sprintf(buf_name, "BUFFER");
	strcpy(so->object_name, buf_name);
	so->object = (void *) 0;
	so->object_count = count;
	so->object_size = sizeof(struct shared_buffer) + (sizeof(struct buffer)+size) * count;

	so->init = init_shared_buffer;
	so->cleanup = destroy_shared_buffer;

	struct shared_object *pso = create_shared_object(so);
	printf("inside %s:%s\n", __FILE__, __func__);
	dump_so(pso);

	return pso;
}

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

	struct shared_object *pso = create_and_init_shared_buffer(size, count);
	printf("inside %s:%s\n", __FILE__, __func__);
	dump_so(pso);

	parse_file_and_share(filename, pso, 32);

	free(pso);
	return 0;
}
