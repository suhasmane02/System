#ifndef _SHARED_BUFFER_
#define _SHARED_BUFFER_

// ******************** SHARED BUFFER ********************* /
struct buffer {
	pthread_mutex_t pmutex;
	char buff[0];
};

struct shared_buffer {
	int current_in_use;
	int size_of_each;
	int count;
	int temp;
	struct buffer buffers[0];
};

int init_mutex(void *);
int destroy_mutex(void *);
int init_shared_buffer(void *);
int destroy_shared_buffer(void *);
struct shared_object* create_and_init_shared_buffer(int, int, short);

#endif
