CC = gcc -std=gnu99
CFLAGS =
LDLIBS = -lm

progress: progress.o
.PHONY: clean
clean:
	$(RM) progress progress.o
