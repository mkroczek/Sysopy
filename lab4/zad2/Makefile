CC = gcc
CFLAGS = -Wall -g

all: zad2

zad2: zad2.c
	$(CC) $(CFLAGS) $^ -o $@

test: zad2
	./zad2 SIGINFO
	./zad2 RESETHAND
	./zad2 NOCLDSTOP
	make clean

clean:
	rm zad2