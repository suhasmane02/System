CC = gcc
CFLAGS = -Wall -g $(DEBUG)
LIBS = -lrt -pthread
TARGET1 = producer
TARGET2 = consumer
TARGET3 = dump_shared_memory
TARGET = $(TARGET1) $(TARGET2)
COMMON = shared_object.c shared_buffer.c
SRCS1 = producer.c
SRCS2 = consumer.c
SRCS3 = dump_shared_memory.c
OBJS1 = $(SRCS1:.c=.o)
OBJS2 = $(SRCS2:.c=.o)
OBJS3 = $(SRCS3:.c=.o)
COMMONOBJS = $(COMMON:.c=.o)
RM = rm
CORE = core*

all: $(TARGET1) $(TARGET2) $(TARGET3)

$(TARGET1): $(OBJS1) $(COMMONOBJS)
	$(CC) $(CFLAGS) -o $@  $(OBJS1) $(COMMONOBJS) $(LIBS)

$(TARGET2): $(OBJS2) $(COMMONOBJS)
	$(CC) $(CFLAGS) -o $@  $(OBJS2) $(COMMONOBJS) $(LIBS)

$(TARGET3): $(OBJS3) $(COMMONOBJS)
	$(CC) $(CFLAGS) -o $@  $(OBJS3) $(COMMONOBJS) $(LIBS)

clean:
	$(RM) $(TARGET1) $(TARGET2) $(TARGET3)  $(OBJS1) $(OBJS2) $(OBJS3) $(COMMONOBJS) $(CORE) /dev/shm/BUFFER
