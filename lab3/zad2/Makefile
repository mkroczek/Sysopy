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
	./static_libtest test merge_files 3 AS.txt:BS.txt AS.txt:CS.txt BS.txt:CS.txt numbers_of_blocks 3 10 500 1000 files_length short > raport_fork.txt
	./static_libtest test merge_files 3 AM.txt:BM.txt AM.txt:CM.txt BM.txt:CM.txt numbers_of_blocks 3 10 500 1000 files_length medium >> raport_fork.txt
	./static_libtest test merge_files 3 AL.txt:BL.txt AL.txt:CL.txt BL.txt:CL.txt numbers_of_blocks 3 10 500 1000 files_length long >> raport_fork.txt
	make clean

# static_libtest: libtest.c libstaticmerge.a
# 	$(CC) $(CFLAGS) -o $@ $^ -L . -lstaticmerge

clean:
	rm *.o *.a static_libtest console