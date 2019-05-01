#include <stdio.h>
#include <string.h>

#include "scraper.h"
#include "httpClient.h"


int main(int argc, char ** argv)
{

	int debug = 1;

    //char test[]="1BvBMSEYstWetqTFn5Au4m4GFg7xJaNVN2,3J98t1WpEZ73CNmQviecrnyiWrnqRhWNLy,bc1qar0srrr7xfkvy5l643lydnw9re59gtzzwf5mdq,,d";





	

	struct sockaddr socketInfo;
	int method = 0;
	//char URL[]="https://www.google.es:443/algo/loquesea/esunpath";
	//char URL[]="https://www.google.es:443";
	//char URL[]="https://www.google.es/";
	//char URL[]="https://www.meneame.net/";
	//char URL[]="https://www.microsoft.com/";

	char URL[]="https://www.blockchain.com/btc/popular-addresses";
	//char URL[]="https://www.blockchain.com";

	int extractResult = extractSocketInfoFromURL(&socketInfo, &method, URL, debug);

	if(extractResult)
	{
		if(debug)
		{
			printf("Cannot process URL info\n");
		}
		return -1;
	}


	int urlResult = 0;
	int pathBufferSize = 1000;
	char pathBuffer[pathBufferSize];
	
	urlResult = extractPathFromURL(URL, pathBuffer, pathBufferSize, 1);

	if(urlResult < 0)
	{
		printf("Could not extract path from URL, return code is %i\n",urlResult);
		exit(-1);
	}

	printf("Extracted path is %s\n",pathBuffer);

    int resultLength = 0;
    int resultCode = 0;


	SecureSocket *secureSocket;

	secureSocket = openSecureSocket(method, &socketInfo);

	if(!secureSocket)
	{
		printf("Cannot open secure socket\n");
		return -1;
	}

	//SACAR LA PARTE DE RUTA DE LA URL DE LA DIRECCIÓN
	char httpRequest[pathBufferSize+17];
	//sprintf(httpRequest,"GET %s HTTP/1.1\r\n\r\n",pathBuffer);
	//strcpy(httpRequest, "GET /btc/popular-addresses\r\n\r\n");	
	strcpy(httpRequest, "GET /btc/popular-addresses HTTP/1.1\nHost: www.blockchain.com\r\n\r\n");	
	printf("Request string is %s\n", httpRequest);
	printf("Request string length is %li\n",strlen(httpRequest));
	printf("Writing to server...\n");
	int writeResult = writeToServer(secureSocket, httpRequest,strlen(httpRequest));
	printf("Ended writing to server...\n");


	if(writeResult < strlen(httpRequest))
	{
		//VER SI ESTE ERROR REALMENTE SE PUEDE DAR.
	}

	int maxDataLength = 10000;
	int totalBufferSpace = maxDataLength+1;
	char *readData=calloc(totalBufferSpace,1); //Take \0 into account since we are reading a string
	int readResultLength = 0;
	int totalReadBytes = 0;
	printf("Starting read process from server...\n");
	do
	{
		printf("Reading from server...\n");
		readResultLength = readFromServer(secureSocket, readData + totalReadBytes, maxDataLength);
		printf("Reading from server returned...\n");
		printf("Read %i bytes\n",readResultLength);
		if(readResultLength < 0)
		{
			free(readData);
			return -1;	
		}
		
		totalReadBytes += readResultLength;

		if(totalReadBytes >= totalBufferSpace)
		{
			printf("Total Read bytes is %i\n",totalReadBytes);			
			totalBufferSpace = totalReadBytes + maxDataLength;
			printf("Next size for realloc is %i\n",totalBufferSpace);
			readData = realloc(readData,totalBufferSpace);
			printf("Reallocated for %i buffer space\n",totalBufferSpace);
			if(!readData)
			{
				return -1;
			}
		}
	//printf("%s\n",readData);
	}
	while(readResultLength > 100); PONER OTRA CONDICION DE PARADA

	printf("Total read bytes is %i\n",totalReadBytes);
fflush(stdout);
	readData[totalReadBytes] = '\0'; //End the string with the null character. Remember we allocated
	printf("Total string size of read data is %li\n",strlen(readData));
fflush(stdout);

	//printf("Printing web server response\n");
	//printf("\n\n\n%s\n\n\n",readData);


	//char **returnedValues = scrapBTCAddresses(test,&resultLength,&resultCode,0);

	char **returnedValues = scrapBTCAddresses(readData,&resultLength,&resultCode,0);

    printf("\n\n\nPrinting results:\n\n\n");

    for(int i = 0; i < resultLength; i++)
    {

        
        printf("%s\n",returnedValues[i]);
    }

//	closeSecureSocket(secureSocket);
//	free(readData);
//	freeBTCAddressesArray(returnedValues, resultLength);
	    


}
