#include "cryptopp/cryptlib.h"
#include "cryptopp/ecp.h"
#include "cryptopp/eccrypto.h"
#include "cryptopp/sha.h"
#include "cryptopp/osrng.h"
#include "cryptopp/oids.h"
#include <iostream>

#include "signatureCheck.h"

using namespace std;

bool checkSignature(ECDSA<ECP, SHA256>::PrivateKey &privateKey,ECDSA<ECP, SHA256>::PublicKey &publicKey)
{
    //Global output result
    bool result = false;

    //Creating signer and verifier
    ECDSA<ECP, SHA256>::Signer signer(privateKey);
    AutoSeededRandomPool prng;
    ECDSA<ECP, SHA256>::Verifier verifier(publicKey);

    //Create sample message and string to store signature
    string message = "Yoda said, Do or do not. There is no try.";
    string signature;

 
    StringSource sss( message, true /*pump all*/,
        new SignerFilter( prng,
            signer,
            new StringSink( signature )
        ) // SignerFilter
    ); // StringSource

 

    StringSource ssv( signature+message, true /*pump all*/,
        new SignatureVerificationFilter(
            verifier,
            new ArraySink( (byte*)&result, sizeof(result) )
        ) // SignatureVerificationFilter
    );


    return result;
}