CC = /usr/bin/gcc

C_SRC = $(wildcard src/*.c)
HEADERS = $(wildcard headers/*.h)
OBJ = $(patsubst src/%.c, obj/%.o, $(C_SRC))

CFLAGS = -std=c17 -Wall -Wextra -pedantic

run: compile
	./bin/namsh

compile: bin/namsh

bin/namsh: $(OBJ) | bin
	$(CC) -o $@ $(OBJ)

bin:
	mkdir -p bin

obj/%.o: src/%.c | obj
	${CC} -c $< -o $@

obj:
	mkdir -p obj

clean:
	rm -f bin/* obj/*.o
