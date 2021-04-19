CC = gcc
CFLAGS = -Wall -g

all: main

consumer: consumer.c
	$(CC) $(CFLAGS) $^ -o $@

producer: producer.c
	$(CC) $(CFLAGS) $^ -o $@

main: main.c producer consumer
	$(CC) $(CFLAGS) $^ -o $@

test: main
	./main > wnioski.txt
	make clean

clean:
	rm main consumer producer