CC = gcc
CFLAGS = -Wall -g

all: pizzeria

cook: cook.c config.c
	$(CC) $(CFLAGS) $^ -o $@

deliverer: deliverer.c config.c
	$(CC) $(CFLAGS) $^ -o $@

pizzeria: pizzeria.c config.c cook deliverer
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm pizzeria cook deliverer