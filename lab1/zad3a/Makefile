CC = gcc
CFLAGS = -Wall -g

all: test_3a

libmerge.o: libmerge.c libmerge.h
	$(CC) $(CFLAGS) -c libmerge.c

libstaticmerge.a: libmerge.o
	ar rcs libstaticmerge.a libmerge.o

libsharedmerge.so: libmerge.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ libmerge.c

console: libtest.c libstaticmerge.a
	$(CC) $(CFLAGS) -o $@ $^ -L . -lstaticmerge

static_test: libtest.c libstaticmerge.a
	$(CC) $(CFLAGS) -o $@ $^ -L . -lstaticmerge
	./static_test test >> raport3a.txt

shared_test: libtest.c 
	make libsharedmerge.so
	$(CC) $(CFLAGS) -o $@ $^ -L . -lsharedmerge -lc -Wl,-rpath=.
	./shared_test test >> raport3a.txt

dynamic_test: dynamic_libtest.c libsharedmerge.so
	make libsharedmerge.so
	$(CC) $(CFLAGS) -o $@ $^ -Wl,-rpath=. -ldl
	./dynamic_test test >> raport3a.txt

test_3a:
	echo "STATIC\n" > raport3a.txt
	make static_test

	echo "\nSHARED\n" >> raport3a.txt
	make shared_test

	echo "\nDYNAMIC\n" >> raport3a.txt
	make dynamic_test

	make clean

clean:
	rm *.o *.a *.so *.out static_test shared_test dynamic_test console