CC = gcc
CFLAGS = -Wall -g

all: client server

client: client.c keys.c que_utils.c
	$(CC) $(CFLAGS) $^ -o $@ -lrt

server: server.c keys.c que_utils.c
	$(CC) $(CFLAGS) $^ -o $@ -lrt

clean:
	rm server client