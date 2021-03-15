CC = gcc
CFLAGS = -Wall -g
BINS = libstaticmerge.a static_libtest

all: $(BINS)

libmerge.o: libmerge.c libmerge.h
	$(CC) $(CFLAGS) -c libmerge.c

libstaticmerge.a: libmerge.o
	ar rcs libstaticmerge.a libmerge.o

static_libtest: libtest.c 
	$(CC) $(CFLAGS) -o $@ $^ -L . -lstaticmerge
	./static_libtest > raport2.txt
	make clean

clean:
	rm *.o $(BINS)