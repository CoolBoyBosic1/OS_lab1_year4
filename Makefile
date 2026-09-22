# Складання без CLion: make && make test
CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Werror -O2
SRCS    = $(wildcard src/*.c)
OBJS    = $(SRCS:.c=.o)

wish: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

src/%.o: src/%.c src/shell.h
	$(CC) $(CFLAGS) -c $< -o $@

test: wish
	./test-wish.sh -c

clean:
	rm -f $(OBJS) wish
	rm -rf tests-out

.PHONY: test clean
