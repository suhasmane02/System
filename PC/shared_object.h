#ifndef _SHARED_OBJECT_H_
#define _SHARED_OBJECT_H_

#define	SUCCESS	0
#define	OBJECT_INIT_FAILED	-1
#define OBJECT_CLEANUP_FAILED	-2
#define INTERNAL_ERROR	-3

struct shared_object {
        char object_name[16];
	int object_count;
        int object_size;
        int (*init)(void*);
        int (*cleanup)(void*);
        int error_code;
        void *object;
};

void dump_so(struct shared_object *);
struct shared_object* create_shared_object(struct shared_object *);
struct shared_object* destroy_shared_object(struct shared_object *);

#endif
