CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lrt

PROGS = kernel app intercontroller

all: $(PROGS)

%: %.c common.h
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

run: all
	./kernel 3

clean:
	rm -f $(PROGS)

.PHONY: all run clean
