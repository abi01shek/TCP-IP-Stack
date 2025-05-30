CC=gcc
CFLAGS=-g -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Werror=return-type -Wextra -Wpedantic
LDFLAGS=
SRCS = gluethread/glthread.c net.c graph.c topologies.c main.c utils.c
OBJS = $(SRCS:.c=.o)
EXECUTABLE = main

all: $(EXECUTABLE)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

clean:
	rm -f $(OBJS) $(EXECUTABLE)
