CC = gcc -std=gnu99
CFLAGS = -Wall -Wextra -O3
LDLIBS = -lm

example: progress.o example.o

.PHONY: clean
clean:
	$(RM) progress progress.o example.o
