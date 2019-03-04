#include "cryptopp/cryptlib.h"
#include "cryptopp/ecp.h"
#include "cryptopp/eccrypto.h"
#include "cryptopp/sha.h"
#include "cryptopp/osrng.h"
#include "cryptopp/oids.h"
#include <iostream>

#include "signatureCheck.h"

using namespace std;

bool checkKeys(ECDSA<ECP, SHA256>::PrivateKey &privateKey,ECDSA<ECP, SHA512>::PublicKey &publicKey)
{

	const Integer& x = privateKey.GetPrivateExponent();


cout << "Private Key Exponent: " << std::hex << x<< endl;


const ECP::Point& q = publicKey.GetPublicElement();
  // en binario en binario!!! XD
  // const EC2N::Point& q = publicKey.GetPublicElement();

  const Integer& qx = q.x;
  const Integer& qy = q.y;
  // en binario en binario!!! XD
  // const std::bitset <256> qx = q.x;
  // const std::bitset <256> qy = q.y;

  //cout << "q.x " << std::hex << qx << endl;
  //cout << "q.y " << std::hex << qy << endl;

    cout << "Public Key X: " << std::hex << qx  << endl;
    cout << "Public Key Y: " << std::hex << qy  << endl;



    return false;
}