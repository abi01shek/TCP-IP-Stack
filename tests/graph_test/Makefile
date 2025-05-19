objects = main

CFLAGS=-g -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Werror=return-type -Wextra -Wpedantic

all: $(objects)

$(objects): %: %.c
	    $(CC) $(CFLAGS) -o $@ $< ../../graph.c ../../gluethread/glthread.c -I../../gluethread

clean:
	rm $(objects)
