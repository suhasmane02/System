#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "shared_object.h"

void dump_so(struct shared_object *so)
{
#ifdef DEBUG
	printf("DUMPING SHARED OBJECT\n");
	printf("so = %p\n", so);
	printf("so->object_name = %s\n", so->object_name);
	printf("so->object_count = %d\n", so->object_count);
	printf("so->object_size = %d\n", so->object_size);
	printf("so->object = %p\n\n", so->object);
#else
	return;
#endif
}

struct shared_object* create_shared_object(struct shared_object *so)
{
	int shm_fd;

	so->error_code = 0;

	shm_fd = shm_open(so->object_name, O_CREAT | O_RDWR, 0666);
	if(shm_fd < 0)
	{
		perror("failure on shm_open on shm_fd");
		so->error_code = INTERNAL_ERROR;
		return so;
	}

	int size = so->object_size * so->object_count;

	if(ftruncate(shm_fd, size) == -1)
	{
		perror("error on ftruncate to truncate to provided size");
		so->error_code = INTERNAL_ERROR;
		return so;
	}

	printf("b4 mmap inside %s:%s\n", __FILE__, __func__);
	dump_so(so);
	so->object = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	printf("8r mmap inside %s:%s\n", __FILE__, __func__);
	dump_so(so);

	if(so->object == MAP_FAILED)
	{
		perror("Error on mmap");
		so->error_code = INTERNAL_ERROR;
		return so;
	}

	if(so->init && (so->init(so) == -1))
	{
		printf("failed to initialize the object\n");
		so->error_code = OBJECT_INIT_FAILED;
		return (so);
	}

	return so;
}

struct shared_object* destroy_shared_object(struct shared_object *so)
{
	so->error_code = 0;
	if(so->cleanup && (so->cleanup(so) == -1))
	{
		printf("shared object destroy failed\n");
		so->error_code = OBJECT_CLEANUP_FAILED;
	}
	shm_unlink(so->object_name);

	return so;
}
