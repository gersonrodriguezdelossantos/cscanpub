using namespace CryptoPP;

bool checkSignature(ECDSA<ECP, SHA256>::PrivateKey &privateKey,ECDSA<ECP, SHA512>::PublicKey &publicKey);