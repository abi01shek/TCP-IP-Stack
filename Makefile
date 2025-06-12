CC=gcc
CFLAGS=-g -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Werror=return-type -Wextra -Wpedantic
LDFLAGS=
LIBS = -lpthread -L CommandParser -lcli
SRCS = gluethread/glthread.c net.c graph.c topologies.c main.c
OBJS = $(SRCS:.c=.o)
EXECUTABLE = main

all: $(EXECUTABLE)
	(cd CommandParser; make)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJS) CommandParser/libcli.a
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)

CommandParser/libcli.a:
	(cd CommandParser; make)

clean:
	rm -f $(OBJS) $(EXECUTABLE)
	(cd CommandParser; make clean)
