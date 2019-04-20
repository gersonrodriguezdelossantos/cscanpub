#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <regex.h>
#include <string.h>

//Define MSG_NOSIGNAL to compile in non-supported platforms
#ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL      0x0000 //In other platforms MSG_NOSIGNAL is 0x2000, but we are zeroing it to prevent undesired effects, should bit patterns be different in other platforms
#endif
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
//Function to negotiate a secure SSL/TLS session for a socket
int negotiateSecureSession(SecureSocket *secureSocket);
//Function to close a socket
int closeSecureSocket(SecureSocket *secureSocket);
//Function to close a secure session
int closeSecureSession(SecureSocket *secureSocket);
//Function to write to a common or SSL socket once a connection is open
int writeToServer(SecureSocket *secureSocket, char *data, int dataLength);
//Function to read from a common or SSL socket once a connection is open
int readFromServer(SecureSocket *secureSocket, char *data, int dataLength);

//Function to extract IP Address from URL
int extractSocketInfoFromURL(struct sockaddr *socketInfo, int *securityMethod, char *URL, int debug);

//Function to extract the path part from an http URL
int extractPathFromURL(char *URL, char *path, int maxPathLength, int debug);


