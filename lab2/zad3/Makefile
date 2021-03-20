CC = gcc
CFLAGS = -Wall -g

all: zad3sys zad2lib

zad2sys: zad3sys.c
	$(CC) $(CFLAGS) $^ -o $@

zad2lib: zad3lib.c
	$(CC) $(CFLAGS) $^ -o $@

clean: 
	rm zad3sys zad3lib