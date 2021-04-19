CC = gcc
CFLAGS = -Wall -g

all: main

consumer: consumer.c
	$(CC) $(CFLAGS) $^ -o $@

producer: producer.c
	$(CC) $(CFLAGS) $^ -o $@

main: main.c producer consumer
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm main consumer producer