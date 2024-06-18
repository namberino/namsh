CC = /usr/bin/gcc

compile: 
	$(CC) namsh.c -o namsh

run:
	$(CC) namsh.c -o namsh
	./namsh
