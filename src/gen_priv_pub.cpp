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

#include "signatureCheck.h"


using std::cout;
using std::endl;

int main()
{

  using namespace CryptoPP;

  AutoSeededRandomPool prng;
  ECDSA<ECP, SHA256>::PrivateKey privateKey;
  // en binario en binario!!! XD
  // ECDSA<EC2N, SHA256>::PrivateKey privateKey;

  privateKey.Initialize( prng, ASN1::secp256r1() );

  const Integer& x = privateKey.GetPrivateExponent();

  cout << "exp " << std::hex << x << endl;

  // const Integer& publickKey = 0;
  ECDSA<ECP, SHA256>::PublicKey publicKey;
  // en binario en binario!!! XD
  // ECDSA<EC2N, SHA512>::PublicKey publicKey;

  //Derive public key
  privateKey.MakePublicKey(publicKey);


  const ECP::Point& q = publicKey.GetPublicElement();
  // en binario en binario!!! XD
  // const EC2N::Point& q = publicKey.GetPublicElement();

  const Integer& qx = q.x;
  const Integer& qy = q.y;
  // en binario en binario!!! XD
  // const std::bitset <256> qx = q.x;
  // const std::bitset <256> qy = q.y;

  cout << "q.x " << std::hex << qx << endl;
  cout << "q.y " << std::hex << qy << endl;
  // en binario en binario!!! XD
  // cout << std::bitset << qx;
  // cout << std::bitset << qy << endl;

  checkKeys(privateKey,publicKey);

}
