CC=gcc
CFLAGS=-g -Wall -Wextra -Werror -pedantic -Wpadded
CFLAGSO=-Wall -Wextra -pedantic -O3 -flto #Optimized CFLAGS
STD=-std=c89
HEADERS=genetics.h
OBJECTS=genetics.o

%.o : %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS) $(STD)

tepal: $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(STD)

clean:
	rm tepal *.o
