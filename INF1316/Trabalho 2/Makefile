CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

PROG = simvirtual

all: $(PROG)

$(PROG): simvirtual.c simvirtual.h
	$(CC) $(CFLAGS) simvirtual.c -o $(PROG)

clean:
	rm -f $(PROG)

.PHONY: all clean
