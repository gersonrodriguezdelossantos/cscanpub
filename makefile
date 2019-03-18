CC=gcc
GG=g++
GGFLAGS=-L/usr/local/lib -lcryptopp -I/usr/local/include
DEPS = src/scraper.h

all: gen_priv_pub scraper

scraper: src/scraper.c
	$(CC) -Wall -o scraperTest src/scraper.c src/test-scraper.c 

gen_priv_pub: src/gen_priv_pub.cpp
	$(GG) -g3 -O2 -Wall -Wextra -o gen_priv_pub src/gen_priv_pub.cpp src/signatureCheck.cpp $(GGFLAGS)

clean:
	rm -f src/*.o src/*.cpp src/*.c
