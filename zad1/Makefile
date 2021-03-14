CC = gcc
CFLAGS = -Wall -g
BINS = libtest libstaticmerge.a libmerge.so static_libtest shared_libtest

all: $(BINS)

libmerge.o: libmerge.c libmerge.h
	$(CC) $(CFLAGS) -c libmerge.c

libmerge.so: libmerge.c libmerge.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ libmerge.c -lc

libstaticmerge.a: libmerge.o
	ar rcs libstaticmerge.a libmerge.o

libtest: libtest.c libmerge.o
	$(CC) $(CFLAGS) -o $@ $^

shared_libtest: libtest.c
	$(CC) $(CFLAGS) -o $@ $^ -L . -lmerge

static_libtest: libtest.c
	$(CC) $(CFLAGS) -o $@ $^ -L . -lstaticmerge

clean:
	rm *.o $(BINS)