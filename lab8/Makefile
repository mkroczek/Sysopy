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

times: zad1
	./zad1 1 numbers input/balloons.ascii.pgm output/balloons_numbers.ascii.pgm > Times.txt
	./zad1 2 numbers input/balloons.ascii.pgm output/balloons_numbers.ascii.pgm >> Times.txt
	./zad1 4 numbers input/balloons.ascii.pgm output/balloons_numbers.ascii.pgm >> Times.txt
	./zad1 8 numbers input/balloons.ascii.pgm output/balloons_numbers.ascii.pgm >> Times.txt
	./zad1 16 numbers input/balloons.ascii.pgm output/balloons_numbers.ascii.pgm >> Times.txt

	./zad1 1 block input/balloons.ascii.pgm output/balloons_numbers.ascii.pgm >> Times.txt
	./zad1 2 block input/balloons.ascii.pgm output/balloons_numbers.ascii.pgm >> Times.txt
	./zad1 4 block input/balloons.ascii.pgm output/balloons_numbers.ascii.pgm >> Times.txt
	./zad1 8 block input/balloons.ascii.pgm output/balloons_numbers.ascii.pgm >> Times.txt
	./zad1 16 block input/balloons.ascii.pgm output/balloons_numbers.ascii.pgm >> Times.txt

	./zad1 1 numbers input/casablanca.ascii.pgm output/casablanca_numbers.ascii.pgm >> Times.txt
	./zad1 2 numbers input/casablanca.ascii.pgm output/casablanca_numbers.ascii.pgm >> Times.txt
	./zad1 4 numbers input/casablanca.ascii.pgm output/casablanca_numbers.ascii.pgm >> Times.txt
	./zad1 8 numbers input/casablanca.ascii.pgm output/casablanca_numbers.ascii.pgm >> Times.txt
	./zad1 16 numbers input/casablanca.ascii.pgm output/casablanca_numbers.ascii.pgm >> Times.txt

	./zad1 1 block input/casablanca.ascii.pgm output/casablanca_numbers.ascii.pgm >> Times.txt
	./zad1 2 block input/casablanca.ascii.pgm output/casablanca_numbers.ascii.pgm >> Times.txt
	./zad1 4 block input/casablanca.ascii.pgm output/casablanca_numbers.ascii.pgm >> Times.txt
	./zad1 8 block input/casablanca.ascii.pgm output/casablanca_numbers.ascii.pgm >> Times.txt
	./zad1 16 block input/casablanca.ascii.pgm output/casablanca_numbers.ascii.pgm >> Times.txt

clean:
	rm zad1