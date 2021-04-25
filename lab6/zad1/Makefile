CC = gcc
CFLAGS = -Wall -g

all: server

client: client.c keys.c que_utils.c
	$(CC) $(CFLAGS) $^ -o $@

server: server.c keys.c que_utils.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm server client