CC=gcc
CFLAGS=-c -Wall -O2 -g

all: test

execute_rrip: rrip
	./rrip

rrip: main_rrip.o rrip.o
	$(CC) main_rrip.o rrip.o -o rrip

main_rrip.o: rrip_src/main_rrip.c
	$(CC) $(CFLAGS) rrip_src/main_rrip.c

rrip.o: rrip_src/rrip.c
	$(CC) $(CFLAGS) rrip_src/rrip.c

execute_lru: lru
	./lru

lru: main_lru.o lru.o
	$(CC) main_lru.o lru.o -o lru

main_lru.o: lru_src/main_lru.c
	$(CC) $(CFLAGS) lru_src/main_lru.c

lru.o: lru_src/lru.c
	$(CC) $(CFLAGS) lru_src/lru.c

test: rrip lru
	./tests/bash.sh
clean:
	rm *.o rrip lru