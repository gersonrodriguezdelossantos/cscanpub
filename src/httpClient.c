#include "httpClient.h"

//Function to Request an HTTP/HTTPS webpage
char *requestPage(char *URL)
{

	return NULL;
}

//Function to obtain a socket


SecureSocket *openSecureSocket(int method, struct sockaddr *socketInfo)
{
    //Sanity check
    if( !socketInfo || (method != INSECURE_METHOD && method != SECURE_METHOD) )
    {
        return NULL;
    }


	if(socketInfo -> sa_family != AF_INET && socketInfo -> sa_family != AF_INET6 )
	{
		return NULL;
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

//Function to negotiate a secure SSL/TLS session for a socket
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
		int associateError = SSL_set_fd(secureSocket -> ssl, secureSocket -> tcpSocket);
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

	printf("SecureSocket pointer is %p\n",secureSocket);
	printf("SecureSocket TCP socket number is %i\n",secureSocket -> tcpSocket);
	printf("SecureSocket SSL pointer is %p\n",secureSocket -> ssl);
	printf("Data buffer has length %i and dataLength parameter is %i\n",strlen(data),dataLength);
	printf("Buffer content is:\n#%s#\n",data);
	fflush(stdout);

    int bytesWritten = 0;
    if(secureSocket -> ssl && secureSocket -> tcpSocket > -1)
    {
		printf("Writing bytes to socket...\n");
		fflush(stdout);
        bytesWritten = SSL_write(secureSocket -> ssl, data, dataLength);
		printf("Written bytes to socket...\n");
		fflush(stdout);
        if (bytesWritten < 0)
        {
			printf("Error during write operation to server, return code is %i\n",bytesWritten);
            int sslError = SSL_get_error(secureSocket -> ssl, bytesWritten);
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
		printf("Wrote %i bytes to server\n",bytesWritten);
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
            int sslError = SSL_get_error(secureSocket -> ssl, bytesRead);
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
http://www.bichito-bueno.com:343/hola/loquesea
https://www.bichito-malo.com:3434/ruta/MAYUSCULAS




*/

	//char regexPattern[] = "http[s]*://(.*)/";
	//THERE COULD BE HYPENS IN THE HOSTNAME, UPDATE REGEX!!!
	//ESCAPE / WITH \\/, SINCE / IS A DELIMITER
	char regexPattern[] = "(http|https)://([a-zA-Z\\.0-9]+)(:[0-9]+)*[\\/]*";
	//Otro patrón para cazar números de puerto: (http|https):\/\/([a-zA-Z\.0-9]+)(:[0-9]+)*[\/]*
	//Quitando las secuencias de escape para las "/" queda: (http|https)://([a-zA-Z\.0-9]+)(:[0-9]+)*[/]*
	int returnedCode = 0;

	//Compile the regex
	regex_t regex;
	returnedCode = regcomp(&regex,regexPattern,REG_EXTENDED);
	if (returnedCode)
	{
		if(debug)
		{
		    printf(stderr, "Cannot compile URL regex!!\n");
		}
		return -1;
	}
	else
	{
		if(debug)
		{
		    printf("URL Regex compilation returned SUCCESS!!\n");
		}
	}
	//Structure to store occurrences of the regex
	regmatch_t occurrences[4];

	//Find a substring that matches the regex
	returnedCode = regexec(&regex, URL, 4, occurrences, 0);

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
		printf("URL is %s\n",URL);
		printf("Total regex: rm_so is %ld and rm_eo is %ld\n",occurrences[0].rm_so,occurrences[0].rm_eo);
		
		printf("First capture group rm_so is %ld and rm_eo is %ld\n",occurrences[1].rm_so,occurrences[1].rm_eo);


	}


	//EXTRACT PROTOCOL

	//CHECK THAT INDEED THE PROTOCOL WAS EXTRACTED

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

	if(!strcmp(protocol,"http"))
	{
		if(debug)
		{
			printf("Protocol is http\n");
		}
		*securityMethod = INSECURE_METHOD;
	}
	else if(!strcmp(protocol,"https"))
	{
		if(debug)
		{
			printf("Protocol is http\n");
		}
		*securityMethod = SECURE_METHOD;
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

	//CHECK THAT WE INDEED CAPTURED THE DESIRED CAPTURE GROUP: WITH OLD REGEX WE CAN CAPTURE ONLY THE HOSTNAME. IN OLD REGEX, CAPTURE INDEX IS 1, BUT IN THE NEW ONE IT IS INDEX 2

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
	if(debug)
	{
		printf("Extracting hostname...\n");
		printf("Hostname start index is %i and stop index is %i\n",occurrences[2].rm_so,occurrences[2].rm_eo);
		printf("Hostname length is %i\n",hostnameLength);
	}
	//Allocate memory for the address. Remember that we need to allocate one extra byt for \0!!!
	char hostname[hostnameLength+1];
	hostname[hostnameLength] = 0;
	//Copy the string to the result array.
	//memcpy(hostname,URL+occurrences[1].rm_so,hostnameLength);
	memcpy(hostname,URL+occurrences[2].rm_so,hostnameLength);

	if(debug)
	{
		printf("Hostname is %s\n",hostname);
	}

	//EXTRACT PORT NUMBER
	int portNumber = 0;
	//CHECK THAT INDEED THE PORTNUMBER WAS EXTRACTED
	if(occurrences[3].rm_so != -1)
	{
		if(debug)
		{
			printf("Port number specified, extracting port number...\n");
		}

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

		if(debug)
		{
			printf("Portnumber start index is %i and stop index is %i\n",occurrences[3].rm_so,occurrences[3].rm_eo);
			printf("Portnumber length is %i\n",portNumberLength);
		}

		char portNumberString[6];
		memset(portNumberString,0,sizeof(portNumberString));
		memcpy(portNumberString,URL+occurrences[3].rm_so+1,portNumberLength);

		if(debug)
		{
			printf("Portnumber string is %s\n",portNumberString);

		}

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
		if(debug)
		{
			printf("No port number specified. Using default port numbers...\n");
		}

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

	int resolveError = getaddrinfo(hostname, NULL, NULL, &IPAddress);
    
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

	struct sockaddr_in *IPAddress4;
	struct sockaddr_in6 *IPAddress6;
	if(portNumber > 0)
	{
		if(IPAddress -> ai_family == AF_INET)
		{
			IPAddress4 = (struct sockaddr_in *) IPAddress -> ai_addr;
			IPAddress4 -> sin_port = htons(portNumber);
		}
		else if(IPAddress -> ai_family == AF_INET6)
		{
			IPAddress6 = (struct sockaddr_in6 *) IPAddress -> ai_addr;
			IPAddress6 -> sin6_port = htons(portNumber);
		}
	}
	else
	{
		return -1;
	}
	
	memcpy(socketInfo,IPAddress->ai_addr,sizeof(struct sockaddr));

	if(debug)
	{
		printf("Function extractSocketInfoFromURL finished sucessfully, returning...\n");
	}

	freeaddrinfo(IPAddress);
	return 0;
}

int extractPathFromURL(char *URL, char *path, int maxPathLength, int debug)
{

	//THERE COULD BE HYPENS IN THE HOSTNAME, UPDATE REGEX!!!
	//"http[s]{0,1}:\/\/[a-zA-Z\\.0-9:]+(\/*.*)"
	//"http[s]{0,1}:\/\/[a-zA-Z\\.0-9]+[:0-9]*(\/*.*)" //BETTER

	//THERE COULD BE HYPENS IN THE HOSTNAME, UPDATE REGEX!!!
	//ESCAPE / WITH \\/, SINCE / IS A DELIMITER

	if(debug)
	{
		printf("URL is %s\n",URL);
		printf("URL length is %i\n",strlen(URL));
	}

	char regexPattern[] = "http[s]{0,1}:\\/\\/[a-zA-Z\\.0-9]+[:0-9]*(\\/*.*)"; //BETTER

	int returnedCode = 0;

	//Compile the regex
	regex_t regex;
	returnedCode = regcomp(&regex,regexPattern,REG_EXTENDED);
	if (returnedCode)
	{
		if(debug)
		{
		    printf(stderr, "Cannot compile URL regex!!\n");
		}
		return -1;
	}
	else
	{
		if(debug)
		{
		    printf("URL Regex compilation returned SUCCESS!!\n");
		}
	}
	//Structure to store occurrences of the regex
	regmatch_t occurrences[2];

	//Find a coincidences with the regex
	returnedCode = regexec(&regex, URL, 2, occurrences, 0);

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

	if(occurrences[0].rm_so == -1)
	{
		if(debug)
		{
			printf("Could not match regex correctly\n");
		}
		return -1;
	}

	if(debug)
	{
		printf("Path start index is %i and stop index is %i\n",occurrences[1].rm_so,occurrences[1].rm_eo);
	}

	if(occurrences[1].rm_so == -1 || occurrences[1].rm_so == strlen(URL))
	{
		if(maxPathLength < 2)
		{
			if(debug)
			{
				printf("Result bufferSize is %i, required buffer size is 2, it is not enough\n",maxPathLength);
			}
			return -2; //Insufficient buffer size
		}

		if(debug)
		{
			printf("No path found, extracting default path: /\n");
		}
		strncpy(path,"/",2);
		return 0;
	}

	int capturedPathLength = occurrences[1].rm_eo - occurrences[1].rm_so;

	//We need space in the buffer for the total path and for the terminating null (\0) character
	if(capturedPathLength+1 > maxPathLength)
	{
		if(debug)
		{
				printf("Result bufferSize is %i, required buffer size is %i, it is not enough\n",maxPathLength,capturedPathLength+1);
		}
		return -2; //Insufficient buffer size
	}

	//By using pathLength+1, we will make strncpy to add a \0 character at the end of the string in "path"
	strncpy(path,URL+occurrences[1].rm_so,capturedPathLength+1);

	return 0;


}