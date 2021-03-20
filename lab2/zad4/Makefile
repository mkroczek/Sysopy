CC = gcc
CFLAGS = -Wall -g

all: zad4sys zad4lib

zad4sys: zad4sys.c
	$(CC) $(CFLAGS) $^ -o $@

zad4lib: zad4lib.c
	$(CC) $(CFLAGS) $^ -o $@

clean: 
	rm zad4sys zad4lib