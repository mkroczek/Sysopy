CC = gcc
CFLAGS = -Wall -g

all: zad1a zad1b exec

zad1a: zad1a.c
	$(CC) $(CFLAGS) $^ -o $@

ignore: zad1a
	./zad1a ignore 10
	make clean

handle: zad1a
	./zad1a handle 10
	make clean

mask: zad1a
	./zad1a mask 10
	make clean

pending: zad1a
	./zad1a pendning 10
	make clean

# b)

zad1b: zad1b.c
	$(CC) $(CFLAGS) $^ -o $@

exec: exec.c
	$(CC) $(CFLAGS) $^ -o $@

ignore_exec: zad1b exec
	./zad1b ignore 10
	make clean

mask_exec: zad1b exec
	./zad1b mask 10
	make clean

pending_exec: zad1b exec
	./zad1b pendning 10
	make clean

clean:
	rm zad1a zad1b exec