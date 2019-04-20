#!/bin/bash
gcc -ggdb -Wall -L/usr/local/lib/ -lssl -I/usr/local/include src/scraper.c src/test-scraper.c src/httpClient.c -o scraperTest

