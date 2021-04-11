CC = gcc
CFLAGS = -Wall -g

all: test

main: main.c sender catcher
	$(CC) $(CFLAGS) main.c -o $@

sender: sender.c
	$(CC) $(CFLAGS) $^ -o $@

catcher: catcher.c
	$(CC) $(CFLAGS) $^ -o $@

test: main sender catcher
	./main 5 kill

clean:
	rm main sender catcher