CC = gcc
CFLAGS = -Wall -g

all: test_sigqueue

main: main.c sender catcher
	$(CC) $(CFLAGS) main.c -o $@

sender: sender.c
	$(CC) $(CFLAGS) $^ -o $@

catcher: catcher.c
	$(CC) $(CFLAGS) $^ -o $@

test_kill: main sender catcher
	./main 5 kill

test_sigqueue: main sender catcher
	./main 10 sigqueue

clean:
	rm main sender catcher