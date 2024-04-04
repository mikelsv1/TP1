CC = gcc
CFLAGS = -Wall -Wextra -O3 -fsanitize=address -fsanitize=undefined

all: smht48

smht48: ht48.o smht48.c
	$(CC) $(CFLAGS) -o smht48 smht48.c ht48.o

ht48.o: ht48.h ht48.c
	$(CC) -c ht48.c

try: try.c
	$(CC) $(CFLAGS) -o try try.c

clean:
	rm -f smht48 ht48.o
