CC=gcc
CFLAGS=-g -Wall -Wextra -Werror -pedantic
CFLAGSO=-Wall -Wextra -pedantic -O3 -flto
STD=-std=c89
HEADERS=genetics.h
OBJECTS=genetics.o

%.o : %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS) $(STD)

calyx: $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(STD)

clean:
	rm calyx *.o
