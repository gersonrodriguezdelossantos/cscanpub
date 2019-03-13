#!/bin/bash
arm-linux-gnueabi-g++ -o gen_priv_pub_arm src/gen_priv_pub.cpp src/signatureCheck.cpp -L/usr/local/lib -lcryptopp -I/usr/local/include
