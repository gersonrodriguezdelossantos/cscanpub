// reference
// https://www.cryptopp.com/wiki/Elliptic_Curve_Digital_Signature_Algorithm


#include "cryptopp/cryptlib.h"
#include "cryptopp/ecp.h"
#include "cryptopp/eccrypto.h"
#include "cryptopp/sha.h"
#include "cryptopp/osrng.h"
#include "cryptopp/oids.h"
#include <iostream>
#include <bitset>
#include <iostream>
#include <pthread.h>

#include "signatureCheck.h"

using std::cout;
using std::endl;

#if defined(__cplusplus)
extern "C"
#endif

#define NUM_THREADS 8

void *generator(void*)
{

    cout << "threading...\n";

    AutoSeededRandomPool prng;
    ECDSA<ECP, SHA256>::PrivateKey privateKey;
    // binary!!! XD
    // ECDSA<EC2N, SHA256>::PrivateKey privateKey;

    privateKey.Initialize( prng, ASN1::secp256r1() );

    const Integer& x = privateKey.GetPrivateExponent();

    cout << "exp " << std::hex << x << endl;

    // const Integer& publickKey = 0;
    ECDSA<ECP, SHA256>::PublicKey publicKey;
    // binary!!! XD
    // ECDSA<EC2N, SHA512>::PublicKey publicKey;

    //Derive public key
    privateKey.MakePublicKey(publicKey);


    const ECP::Point& q = publicKey.GetPublicElement();
    // binary!!! XD
    // const EC2N::Point& q = publicKey.GetPublicElement();

    const Integer& qx = q.x;
    const Integer& qy = q.y;
    // binary!!! XD
    // const std::bitset <256> qx = q.x;
    // const std::bitset <256> qy = q.y;

    cout << "q.x " << std::hex << qx << endl;
    cout << "q.y " << std::hex << qy << endl;
    // binary!!! XD
    // cout << std::bitset << qx;
    // cout << std::bitset << qy << endl;


    if(checkSignature(privateKey,publicKey))
    {
      cout << "Keys are mathematically correlated, signature matches!!" << endl;
    }
    else
    {
      cout << "Keys are not mathematically correlated, signature does not match!!" << endl;
    }
}

int main()
{

  using namespace CryptoPP;

  pthread_t threads[NUM_THREADS];

  int rc;
  int i;

  while (true) {
    for( i = 0; i < NUM_THREADS; i++ ) {

      rc = pthread_create(&threads[i], NULL, generator, NULL);

      if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }

    }
  }

  pthread_exit(NULL);
  return 0;

}
