#!/bin/bash
g++ -g3 -O2 -Wall -Wextra -o gen_priv_pub src/gen_priv_pub.cpp src/signatureCheck.cpp -L/usr/local/lib -lcryptopp -I/usr/local/include