CC=gcc
GG=g++
GGFLAGS=-L/usr/local/lib -lcryptopp -lpthread -I/usr/local/include
CCFLAGS=-I. -lssl -lcrypto
DEPS = src/scraper.h

all: gen_priv_pub scraper

scraper: src/scraper.c src/test-scraper.c src/scraper.c src/scraper.h src/httpClient.c src/httpClient.h
	$(CC) -Wall -o scraperTest src/test-scraper.c src/scraper.c src/httpClient.c $(CCFLAGS)

gen_priv_pub: src/gen_priv_pub.cpp
	$(GG) -g3 -O2 -Wall -Wextra -o gen_priv_pub src/gen_priv_pub.cpp src/signatureCheck.cpp $(GGFLAGS)

clean:
	rm -f src/*.o src/*.cpp src/*.c
