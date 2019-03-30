#include <openssl/ssl.h>
#include <openssl/err.h>

//Static variables to define if we will have an HTTP or HTTPS socket
static const int INSECURE_METHOD = 0;
static const int SECURE_METHOD = 1;

//Typedef for a TCP socket with an optional TLS/SSL
typedef struct
{
    int tcpSocket;
    SSL_CTX *sslConnection;
    SSL *ssl;
} SecureSocket;


//Function to Request an HTTP/HTTPS webpage
char *requestPage(char *URL);

//Function to obtain a socket
SecureSocket *openSecureSocket(int method, struct sockaddr *socketInfo);
//Function to close a socket
int closeSecureSocket(SecureSocket *secureSocket);
//Function to close a secure session
int closeSecureSession(SecureSocket *secureSocket);
//Function to write to a common or SSL socket once a connection is open
int writeToServer(SecureSocket *secureSocket, char *data, int dataLength);
//Function to read from a common or SSL socket once a connection is open
int readFromServer(SecureSocket *secureSocket, int dataLength);

//Function to extract IP Address from URL
int extractAddressFromURL(struct sockaddr *socketInfo, int *securityMethod, char *URL, int debug);
