CC = gcc
CFLAGS = -Wall -g

all: server_test

test: test.c utils.c
	$(CC) $(CFLAGS) $^ -o $@
	./test add:5:

utils: utils.c
	$(CC) $(CFLAGS) $^ -o $@

client: client.c utils.c
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

server: server.c utils.c
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

server_test:
	make server
	./server 6666 socket

client_test_local:
	make client
	./client Mati local socket

client_test_network:
	make client
	./client Mati network 127.0.0.1 6666

clean: 
	rm server client socket test