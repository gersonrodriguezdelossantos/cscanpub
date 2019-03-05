#include "cryptopp/cryptlib.h"
#include "cryptopp/ecp.h"
#include "cryptopp/eccrypto.h"
#include "cryptopp/sha.h"
#include "cryptopp/osrng.h"
#include "cryptopp/oids.h"

using namespace CryptoPP;
using namespace std;

bool checkSignature(ECDSA<ECP, SHA256>::PrivateKey &privateKey,ECDSA<ECP, SHA256>::PublicKey &publicKey);
/*
template <class S, class T>
bool bichito(S arg1, T arg2);

template <class ECURVE,class HASH>
bool checkSignatureGeneral();



template <class S, class T>
bool bichito(S arg1, T arg2)
{
    if(arg1 == arg2)
    {
        cout << "MATCHES!!" << endl;
        return true;
    }
    else
    {
        cout << "IT DOES NOT MATCH!!" << endl;
        return false;
    }
    
}



template <class ECURVE,class HASH>
bool checkSignatureGeneral()
{
    //Global output result
    bool result = false;

    ECDSA<ECURVE,HASH>::PublicKey publicKey;
    ECDSA<ECURVE,HASH>::PublicKey privateKey;

    //Creating signer and verifier
    ECDSA<ECURVE,HASH>::Signer signer(privateKey);
    AutoSeededRandomPool prng;
    ECDSA<ECURVE,HASH>::Verifier verifier(publicKey);

    return result;
}
*/