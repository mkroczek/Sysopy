CC = gcc
CFLAGS = -Wall -g

all: zad1sys zad1lib

zad1sys: zad1sys.c
	$(CC) $(CFLAGS) $^ -o $@

zad1lib: zad1lib.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm zad1sys zad1lib
	