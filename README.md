# ECDSA priv and pub key generator

This is a PoC to generate ECDSA private and public keys with C++ and measure performance

It output 2 lines:
* 1: private key in hex
* 2: public key in hex

## Curve and Hashes

* curve used: secp256r1
* hashes: sha256 for the private key and sha512 to derive the public key

## Dependencies

* [crypto++ library](https://www.cryptopp.com)
* [g++ compiler](https://gcc.gnu.org)
