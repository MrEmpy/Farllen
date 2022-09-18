CC = gcc

all:
	$(CC) src/farllen.c -o farllen

i386:
	$(CC) -m32 src/farllen.c -o farllen

clean:
	rm farllen
