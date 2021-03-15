CC = gcc
CFLAGS = -Wall -g
OPT = 0

all: test_3b

libmerge.o: libmerge.c libmerge.h
	$(CC) $(CFLAGS) -c libmerge.c

libstaticmerge.a: libmerge.o
	ar rcs libstaticmerge.a libmerge.o

libsharedmerge.so: libmerge.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ libmerge.c -O$(OPT)

console: libtest.c libstaticmerge.a
	$(CC) $(CFLAGS) -o $@ $^ -L . -lstaticmerge -O$(OPT)

static_test: libtest.c libstaticmerge.a
	$(CC) $(CFLAGS) -o  $@ $^ -L . -lstaticmerge -O$(OPT)
	./static_test test >> raport3b.txt

shared_test: libtest.c 
	make libsharedmerge.so
	$(CC) $(CFLAGS) -o $@ $^ -L . -lsharedmerge -lc -Wl,-rpath=. -O$(OPT)
	./shared_test test >> raport3b.txt

dynamic_test: dynamic_libtest.c libsharedmerge.so
	make libsharedmerge.so
	$(CC) $(CFLAGS) -o $@ $^ -Wl,-rpath=. -ldl -O$(OPT)
	./dynamic_test test >> raport3b.txt

test_3b:
	echo "-O$(OPT) LEVEL TEST\n" >> raport3b.txt

	echo "STATIC\n" >> raport3b.txt
	make static_test

	echo "\nSHARED\n" >> raport3b.txt
	make shared_test

	echo "\nDYNAMIC\n" >> raport3b.txt
	make dynamic_test

	echo "\n\n" >> raport3b.txt

	make clean

clean:
	rm *.o *.a *.so *.out static_test shared_test dynamic_test console