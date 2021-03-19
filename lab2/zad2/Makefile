CC = gcc
CFLAGS = -Wall -g

all: zad2sys zad2lib

zad2sys: zad2sys.c
	$(CC) $(CFLAGS) $^ -o $@

zad2lib: zad2lib.c
	$(CC) $(CFLAGS) $^ -o $@

clean: 
	rm zad2sys zad2lib