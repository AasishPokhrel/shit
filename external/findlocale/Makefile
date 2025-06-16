CC = cc
CFLAGS = -Os -Wall

all: example

example: findlocale.o example.o
	$(CC) example.o findlocale.o -o example

findlocale.o: findlocale.c findlocale.h
	$(CC) $(CFLAGS) -c findlocale.c

example.o: example.c findlocale.h
	$(CC) $(CFLAGS) -c example.c

clean:
	$(RM) findlocale.o example.o example
