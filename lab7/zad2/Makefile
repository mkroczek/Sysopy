CC = gcc
CFLAGS = -Wall -g

all: pizzeria

cook: cook.c config.c
	$(CC) $(CFLAGS) $^ -o $@ -lrt -lpthread

deliverer: deliverer.c config.c
	$(CC) $(CFLAGS) $^ -o $@ -lrt -lpthread

pizzeria: pizzeria.c config.c cook deliverer
	$(CC) $(CFLAGS) $^ -o $@ -lrt -lpthread

clean:
	rm pizzeria cook deliverer