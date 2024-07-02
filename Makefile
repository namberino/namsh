CC = $(shell which gcc)
CC ?= $(shell which clang) # fallback compiler

C_SRC = $(wildcard src/*.c)
HEADERS = $(wildcard headers/*.h)
OBJ = $(patsubst src/%.c, obj/%.o, $(C_SRC))

CFLAGS = -std=c17 -Wall -pedantic -Iinclude

run: compile
	./bin/namsh

compile: bin/namsh

bin/namsh: $(OBJ) | bin
	$(CC) -o $@ $(OBJ)

bin:
	mkdir -p bin

obj/%.o: src/%.c $(HEADERS) | obj
	${CC} $(CFLAGS) -c $< -o $@

obj:
	mkdir -p obj

clean:
	rm -f bin/* obj/*.o
