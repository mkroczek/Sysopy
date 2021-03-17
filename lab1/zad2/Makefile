CC = gcc
CFLAGS = -Wall -g

all: static_libtest

libmerge.o: libmerge.c libmerge.h
	$(CC) $(CFLAGS) -c libmerge.c

libstaticmerge.a: libmerge.o
	ar rcs libstaticmerge.a libmerge.o

console: libtest.c libstaticmerge.a
	$(CC) $(CFLAGS) -o $@ $^ -L . -lstaticmerge

static_libtest: libtest.c libstaticmerge.a
	$(CC) $(CFLAGS) -o $@ $^ -L . -lstaticmerge
	./static_libtest test > raport2.txt
	make clean

clean:
	rm *.o *.a static_libtest console