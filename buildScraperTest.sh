#!/bin/bash
#gcc -ggdb -Wall -lcrypto -lssl -L/usr/lib/x86_64-linux-gnu -I/usr/include src/scraper.c src/test-scraper.c src/httpClient.c -o scraperTest
gcc -ggdb -Wall src/scraper.c src/test-scraper.c src/httpClient.c -o scraperTest -lssl -lcrypto

