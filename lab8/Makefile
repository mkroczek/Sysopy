CC = gcc
CFLAGS = -Wall -g

all: zad1

zad1: zad1.c
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

test: zad1
	./zad1 3 numbers input/balloons.ascii.pgm output/balloons_numbers.ascii.pgm
	./zad1 3 numbers input/casablanca.ascii.pgm output/casablanca_numbers.ascii.pgm
	./zad1 3 block input/balloons.ascii.pgm output/balloons_block.ascii.pgm
	./zad1 3 block input/casablanca.ascii.pgm output/casablanca_block.ascii.pgm

clean:
	rm zad1