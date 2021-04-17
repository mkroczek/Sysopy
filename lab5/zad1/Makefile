CC = gcc
CFLAGS = -Wall -g

all: interpreter

interpreter: interpreter.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm interpreter