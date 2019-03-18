CC=gcc
GG=g++
CFLAGS=-I.
DEPS = scraper.h signatureCheck.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: gen_priv_pubv scraper test-scraper

gen_priv_pub: gen_priv_pub.o gen_priv_pub.cpp
	$(GG) -g3 -O2 -Wall -Wextra -o gen_priv_pub gen_priv_pub.o gen_priv_pub.cpp -L/usr/local/lib -lcryptopp -I/usr/local/include

scraper: scraper.o scraper.c
	$(CC) -Wall -o scraper scraper.o scraper.c

test-scraper: test-scraper.o test-scraper.c
	$(CC) -Wall -o test-scraper test-scraper.o test-scraper.c

clean:
	rm -f *.o *.cpp *.c
