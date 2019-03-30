#include "httpClient.h"

//Function to Request an HTTP/HTTPS webpage
char *requestPage(char *URL);

//Function to obtain a socket

CAMBIARLO PARA QUE SEA SOCKADDR SIN MÁS, PORQUE PUEDE HABER VARIOS TIPOS DE DIRECCIÓN
SecureSocket *openSecureSocket(int method, struct sockaddr *socketInfo)
{
    //Sanity check
    if( !socketInfo || (method != METHOD_NOTSECURE && method != METHOD_SECURE) )
    {
        return NULL;
    }


	if(socketInfo -> sa_family != AF_INET && socketInfo -> sa_family != AF_INET6 )
	{
		return -1;
	}

    //Allocate memory for Secure Socket structure
    SecureSocket *secureSocket=calloc(1,sizeof(SecureSocket));

    if(!secureSocket)
    {
        return NULL;
    }


    //Create socket
    secureSocket -> tcpSocket = socket(socketInfo -> sa_family, SOCK_STREAM, 0);
    if(secureSocket -> tcpSocket == -1)
    {
	free(secureSocket);
        return NULL;
    }
    
    //struct sockaddr_in sa;
    //Open socket to destination address

  
    socklen_t socketLength;

	if(socketInfo -> sa_family == AF_INET)
	{
		socketLength = sizeof(struct sockaddr_in);
	}
	else if(socketInfo -> sa_family == AF_INET6)
	{
		socketLength = sizeof(struct sockaddr_in6);
	}
	else
	{
		//Previous sanity check was made, we should not reach this point
		free(secureSocket);
		return NULL;
	}
    if(connect(secureSocket -> tcpSocket, socketInfo, socketLength))
    {
	free(secureSocket);
        return NULL;
    }

    //If https is necessary, open SSL layer for the socket
    if(method == SECURE_METHOD)
    {
	int returnValue = negotiateSecureSession(secureSocket);

	if(returnValue)
	{
		close(secureSocket -> tcpSocket);
		free(secureSocket);
	}
	

	
    }
    return secureSocket;
}

int negotiateSecureSession(SecureSocket *secureSocket)
{
        //Initialise SSL library
        SSL_library_init(); //Newer versions of OpenSSL do not need explicit initialisation. This call is optional. SSL_library_init is renamed to OPENSSL_init_ssl in newer versions. I include this call for compatibility purposes in case we compile against older versions.
        
        //SSLeay_add_ssl_algorithms(); //This call is unnecessary, since SSLeay_add_ssl_algorithms is a synonim for SSL_library_init

        //Load SSL error strings
        //SSL_load_error_strings(); //Not necessary, we will not be showing error strings


	const SSL_METHOD *sslMethod;
        //Initialise security method. The example followed uses TLSv1_2_client_method, but this one will negotiate the version instead of forcing a specific version that might be unsupported
        sslMethod = TLS_client_method();
        //Open SSL connection
        secureSocket -> sslConnection = SSL_CTX_new(sslMethod);
        //Create the SSL structure
        secureSocket -> ssl = SSL_new(secureSocket -> sslConnection);

	
        if (!(secureSocket -> ssl))
        {
	    //Free allocated resources
	    SSL_CTX_free(secureSocket -> sslConnection);


            return -1;
        }

        //sock = SSL_get_fd(ssl); //I think this line does nothing in the original example and therefore I am commenting it.
        
        //Associate the SSL structure to the socket created
	int associateError = SSL_set_fd(secureSocket -> ssl, tcpSocket);
        if(!associateError)
	{
		//Free allocated resources
		SSL_CTX_free(secureSocket -> sslConnection);
		return -1;

	}

        //Initiate TLS/SSL handshake
        int handshakeError = SSL_connect(secureSocket -> ssl);
        if(handshakeError <= 0)
        {
	    //Free allocated resources
	    SSL_free(secureSocket -> ssl);
	    SSL_CTX_free(secureSocket -> sslConnection);

            return -1;
        }
        
	return 0;
	
}



//Function to close a socket
int closeSecureSocket(SecureSocket *secureSocket)
{

	if(!secureSocket)
	{
		return -1;
	}
	
	
	//Check if we have an SSL socket open
	if(secureSocket -> ssl)
	{
		closeSecureSession(secureSocket);

	}
	//Close socket	
	close(secureSocket -> tcpSocket);

	free(secureSocket);
	return 0;
}
//Function to close a secure session
int closeSecureSession(SecureSocket *secureSocket)
{

	if(!secureSocket -> ssl)
	{
		return -1;
	}

	//Free allocated resources
	SSL_shutdown(secureSocket -> ssl);

	SSL_free(secureSocket -> ssl);
	SSL_CTX_free(secureSocket -> sslConnection);

	return 0;

}
//Function to write to a common or SSL socket once a connection is open
int writeToServer(SecureSocket *secureSocket, char *data, int dataLength)
{
    if(!secureSocket)
    {
        return -1;
    }

    int bytesWritten = 0;
    if(secureSocket -> ssl && secureSocket -> tcpSocket > -1)
    {
        bytesWritten = SSL_write(secureSocket -> ssl, data, dataLength);
        if (bytesWritten < 0)
        {
            int sslError = SSL_get_error(ssl, len);
            switch (sslError)
            {
                case SSL_ERROR_WANT_WRITE:
                    return 0;
		    break;
                case SSL_ERROR_WANT_READ:
                    return 0;
		    break;
                case SSL_ERROR_ZERO_RETURN:
                case SSL_ERROR_SYSCALL:
                case SSL_ERROR_SSL:
                default:
                    return -1;
            }
        }
    }
    else if(secureSocket -> tcpSocket > -1)
    {
	//If this is not a secure socket, read directly from the socket
        bytesWritten = send(secureSocket -> tcpSocket, data, dataLength, MSG_NOSIGNAL);
    }
    else
    {
        return -1;
    }

    return bytesWritten;
    
}
//Function to read from a common or SSL socket once a connection is open
int readFromServer(SecureSocket *secureSocket, char *data, int dataLength)
{
    if(!secureSocket)
    {
        return -1;
    }

    int bytesRead = 0;
    if(secureSocket -> ssl && secureSocket -> tcpSocket > -1)
    {
        bytesRead = SSL_read(secureSocket -> ssl, data, dataLength);
        if (bytesRead < 0)
        {
            int sslError = SSL_get_error(ssl, len);
            switch (sslError)
            {
                case SSL_ERROR_WANT_WRITE:
                    return 0;
		    break;
                case SSL_ERROR_WANT_READ:
                    return 0;
                    break;
                case SSL_ERROR_ZERO_RETURN:
                case SSL_ERROR_SYSCALL:
                case SSL_ERROR_SSL:
                default:
                    return -1;
            }
        }
    }
    else if(secureSocket -> tcpSocket > -1)
    {
	//If this is not a secure socket, read directly from the socket
        bytesRead = recv(secureSocket -> tcpSocket, data, dataLength, 0);

    }
    else
    {
        return -1;
    }

    return bytesRead;

}

//Function to extract IP Address from URL
int extractSocketInfoFromURL(struct sockaddr *socketInfo, int *securityMethod, char *URL, int debug)
{
	
	if(!socketInfo || !URL)
	{
		return -1;
	}

/*
http://www.bichito.com/hola
https://www.bichito.com/hola
http://www.bichito.com:443/
http://www.bichito.com313:45/
http://www.bichito.com313:45
http://www.bichito.com313:45/hola/loquesea
http://www.bichito.com313:45/
https://www.bichito.com313:45
https://www.bichito.com313/hola
http://www.bichito.com313/hola
http://www.bichito.com313/
http://www.bichito.com313



*/

	char regexPattern[] = "http[s]*://(.*)/";
	//Otro patrón para cazar números de puerto: (http|https):\/\/([a-zA-Z\.0-9]+)(:[0-9]+)*[\/]*
	//Quitando las secuencias de escape para las "/" queda: (http|https)://([a-zA-Z\.0-9]+)(:[0-9]+)*[/]*
	int returnedCode = 0;

	//Compile the regex
	returnedCode = regcomp(&regex,regexPattern,REG_EXTENDED);
	if (returnedCode)
	{
		if(debug)
		{
		    fprintf(stderr, "Cannot compile regex!!\n");
		}
		return -1;
	}
	else
	{
		if(debug)
		{
		    printf("Regex compilation returned SUCCESS!!\n");
		}
	}
	//Structure to store occurrences of the regex
	regmatch_t occurrences[2];

	//Find a substring that matches the regex
	returnedCode = regexec(&regex, URL, 2, &occurrences, 0);

        if(returnedCode)
        {
            if(debug)
            {
                    printf("Returned Code is %i\n", returnedCode);


		    int errorMessageLength = 1000;
		    char errorMessage[errorMessageLength];
		    regerror(returnedCode, &regex, errorMessage, errorMessageLength);

	            printf("ERROR MESSAGE IS %s\n", errorMessage);
            }
	    return -1;
        }

	if(debug)
	{
		printf("URL is %s",URL);
		printf("Total regex: rm_so is %ld and rm_eo is %ld\n",occurrences[0].rm_so,occurrences[0].rm_eo);
		
		printf("First capture group rm_so is %ld and rm_eo is %ld\n",occurrences[1].rm_so,occurrences[1].rm_eo);


	}


	//EXTRACT PROTOCOL

	CHECK THAT INDEED THE PROTOCOL WAS EXTRACTED

	if(occurrences[1].rm_so == -1)
	{
		if(debug)
		{
			printf("Cannot find protocol in URL\n");
		}
		return -1;
	}

	int protocolLength = occurrences[1].rm_eo-occurrences[1].rm_so;

	if(protocolLength > 5)
	{
		if(debug)
		{
			printf("Invalid protocol: it must be either http or https\n");
		}
		return -1;
	}

	char protocol[6];
	memset(protocol,0,sizeof(protocol));
	memcpy(protocol,URL+occurrences[1].rm_so,protocolLength);

	if(!strcmp(protocol,http))
	{
		*method = METHOD_NOTSECURE;
	}
	else if(!strcmp(protocol,https))
	{
		*method = METHOD_SECURE;
	}
	else
	{
		if(debug)
		{
			printf("Invalid protocol %s\n",protocol);
		}
		return -1;
	}



	//EXTRACT HOSTNAME

	CHECK THAT WE INDEED CAPTURED THE DESIRED CAPTURE GROUP: WITH OLD REGEX WE CAN CAPTURE ONLY THE HOSTNAME. IN OLD REGEX, CAPTURE INDEX IS 1, BUT IN THE NEW ONE IT IS INDEX 2

	if(occurrences[2].rm_so == -1)
	{
		if(debug)
		{
			printf("Cannot capture hostname: check that hostname is valid or present. It must contain letters, numbers, dots and hyphens\n");
		}
		return -1;
	}

	//Length of hostname
	//int hostnameLength = occurrences[1].rm_eo-occurrences[1].rm_so;
	int hostnameLength = occurrences[2].rm_eo-occurrences[2].rm_so;
	//Allocate memory for the address. Remember that we need to allocate one extra byt for \0!!!
	char hostname[hostnameLength+1];
	hostname[hostnameLength] = 0;
	//Copy the string to the result array.
	//memcpy(hostname,URL+occurrences[1].rm_so,hostnameLength);
	memcpy(hostname,URL+occurrences[2].rm_so,hostnameLength);


	//EXTRACT PORT NUMBER
	int portNumber = 0;
	CHECK THAT INDEED THE PORTNUMBER WAS EXTRACTED
	if(occurrences[3].rm_so != -1)
	{
		//Length of portnumber
		int portNumberLength = occurrences[3].rm_eo-(occurrences[3].rm_so+1);//We do not want the ":" character

		if(portNumberLength > 5)
		{
			if(debug)
			{
				printf("Port number is longer than 5 digits. A port number must be between 1 and 65535\n");
			}
			return -1;
		}

		char portNumberString[6];
		memset(portNumberString,0,sizeof(portNumberString));
		memcpy(portNumberString,URL+occurrences[3].rm_so+1,portNumberLength);


		portNumber = atoi(portNumberString);
		if(portNumber > 65535)
		{
			if(debug)
			{
				printf("Port number is greather than 65535. A port number must be between 1 and 65535\n");
			}
			return -1;
		}
	}
	else
	{
		if(strcmp(protocol,"http"))
		{
			portNumber = 80;
		}
		else if(strcmp(protocol,"https"))
		{
			portNumber = 443;
		}
		else
		{
			//It is unlikely that we will reach this part of the code since the function returns -1 in case neither http or https is used (see code above).
			if(debug)
			{
				printf("Invalid protocol %s. It must be either https or http\n",protocol);
			}
			return -1;
		}
	}


	struct addrinfo* IPAddress;

	resolveError = getaddrinfo(hostname, NULL, NULL, &IPAddress);
    
	if(resolveError)
	{
		if(debug)
		{
			printf("Cannot resolve hostname %s!!\n",hostname);
		}
    	}
	else
	{
		if(debug)
		{
			printf("Successfully resolved hostname!!\n");
		}
	}

	
	if(portNumber > 0)
	{
		if(IPAddress -> sa_family == AF_INET)
		{
			struct sockaddr_in *IPAddress4 = (struct sockaddr_in *) IPAddress;
			IPAddress4 -> sin_port = htons(portNumber);
		}
		else if(IPAddress -> sa_family == AF_INET6)
		{
			struct sockaddr_in6 *IPAddress6 = (struct sockaddr_in *) IPAddress;
			IPAddress6 -> sin6_port = htons(portNumber);
		}
	}
	else
	{
		return -1;
	}
	
	memcpy(socketInfo,IPAddress->ai_addr,sizeof(struct sockaddr));

	return 0;
}
