CC = gcc
CFLAGS = -Wall -g $(DEBUG)
LIBS = -lrt -pthread
TARGET1 = producer
TARGET2 = consumer
TARGET = $(TARGET1) $(TARGET2)
COMMON = shared_object.c
SRCS1 = producer.c
SRCS2 = consumer.c
OBJS1 = $(SRCS1:.c=.o)
OBJS2 = $(SRCS2:.c=.o)
COMMONOBJS = $(COMMON:.c=.o)
RM = rm
CORE = core*

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJS1) $(COMMONOBJS)
	$(CC) $(CFLAGS) -o $@  $(OBJS1) $(COMMONOBJS) $(LIBS)

$(TARGET2): $(OBJS2) $(COMMONOBJS)
	$(CC) $(CFLAGS) -o $@  $(OBJS2) $(COMMONOBJS) $(LIBS)

clean:
	$(RM) $(TARGET1) $(TARGET2) $(OBJS1) $(OBJS2) $(COMMONOBJS) $(CORE) /dev/shm/BUFFER
