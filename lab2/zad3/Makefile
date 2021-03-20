CC = gcc
CFLAGS = -Wall -g

all: zad3sys zad3lib

zad3sys: zad3sys.c
	$(CC) $(CFLAGS) $^ -o $@

zad3lib: zad3lib.c
	$(CC) $(CFLAGS) $^ -o $@

clean: 
	rm zad3sys zad3lib