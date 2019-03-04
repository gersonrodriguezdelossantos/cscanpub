#include "cryptopp/cryptlib.h"
#include "cryptopp/ecp.h"
#include "cryptopp/eccrypto.h"
#include "cryptopp/sha.h"
#include "cryptopp/osrng.h"
#include "cryptopp/oids.h"
#include <iostream>

#include "signatureCheck.h"

using namespace std;

bool checkKeys(ECDSA<ECP, SHA256>::PrivateKey &privateKey,ECDSA<ECP, SHA256>::PublicKey &publicKey)
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

    ECDSA<ECP, SHA256>::Signer signer(privateKey);

    string message = "Yoda said, Do or do not. There is no try.";
    string signature;

    AutoSeededRandomPool prng;
//No funciona
/*
    StringSink ssink( signature );
    SignerFilter sf( prng, signer, ssink);
    StringSource ss(message,true,sf);
*/

StringSource s( message, true /*pump all*/,
    new SignerFilter( prng,
        signer,
        new StringSink( signature )
    ) // SignerFilter
); // StringSource

    cout << "SIGNATURE: " << std::hex << signature << endl;


//####################### VERIFICATION #############

ECDSA<ECP,SHA256>::Verifier verifier(publicKey);

cout << "SIGNATURE LENGTH " << signature.length() << endl;
string falseSignature(signature.length(),'g');
cout << "FALSE SIGNATURE: " << falseSignature << endl;
bool result = false;

StringSource ss1( falseSignature+message, true /*pump all*/,
    new SignatureVerificationFilter(
        verifier,
        new ArraySink( (byte*)&result, sizeof(result) )
    ) // SignatureVerificationFilter
);

if(result)
{
    cout << "SUCCESS" << endl;
}
else
{
    cout << "FAIL" << endl;
}

string falseMessage = "yoda said, Do or do not. There is no try.";
StringSource ss2( signature+falseMessage, true /*pump all*/,
    new SignatureVerificationFilter(
        verifier,
        new ArraySink( (byte*)&result, sizeof(result) )
    ) // SignatureVerificationFilter
);

if(result)
{
    cout << "SUCCESS" << endl;
}
else
{
    cout << "FAIL" << endl;
}


StringSource ss3( signature+message, true /*pump all*/,
    new SignatureVerificationFilter(
        verifier,
        new ArraySink( (byte*)&result, sizeof(result) )
    ) // SignatureVerificationFilter
);

if(result)
{
    cout << "SUCCESS" << endl;
}
else
{
    cout << "FAIL" << endl;
}





    return false;
}