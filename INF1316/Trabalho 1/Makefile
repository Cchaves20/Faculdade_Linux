CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lrt

all: kernel app intercontroller

kernel: kernel.c common.h
	$(CC) $(CFLAGS) kernel.c -o kernel $(LDFLAGS)

app: app.c common.h
	$(CC) $(CFLAGS) app.c -o app $(LDFLAGS)

intercontroller: intercontroller.c common.h
	$(CC) $(CFLAGS) intercontroller.c -o intercontroller $(LDFLAGS)

run: all
	./kernel 3

clean:
	rm -f kernel app intercontroller