CC = gcc
CFLAGS = -Wall -g

all: test_sigrt

main: main.c sender catcher
	$(CC) $(CFLAGS) main.c -o $@

sender: sender.c
	$(CC) $(CFLAGS) $^ -o $@

catcher: catcher.c
	$(CC) $(CFLAGS) $^ -o $@

test_kill: main sender catcher
	./main 500 kill

test_sigqueue: main sender catcher
	./main 500 sigqueue

test_sigrt: main sender catcher
	./main 500 sigrt

clean:
	rm main sender catcher