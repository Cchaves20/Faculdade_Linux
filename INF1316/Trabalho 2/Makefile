CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

PROG = sim-virtual

all: $(PROG)

$(PROG): sim-virtual.c sim-virtual.h
	$(CC) $(CFLAGS) sim-virtual.c -o $(PROG)

clean:
	rm -f $(PROG)

.PHONY: all clean
