#include <stdio.h>

#include "scraper.h"

int main(int argc, char ** argv)
{

	int debug = 1;

    char test[]="1BvBMSEYstWetqTFn5Au4m4GFg7xJaNVN2,3J98t1WpEZ73CNmQviecrnyiWrnqRhWNLy,bc1qar0srrr7xfkvy5l643lydnw9re59gtzzwf5mdq,,d";





	

	struct sockaddr socketInfo;
	int method = 0;
	char URL[]="https://www.example.com";
	int extractResult = extractSocketInfoFromURL(&socketInfo, &method, URL, debug);

	if(extractResult)
	{
		if(debug)
		{
			printf("Cannot process URL info\n");
		}
		return -1;
	}

    int resultLength = 0;
    int resultCode = 0;


 SecureSocket *secureSocket;

	secureSocket = openSecureSocket(method, socketInfo);

	if(!secureSocket)
	{
		printf("Cannot open secure socket\n");
		return -1;
	}

	SACAR LA PARTE DE RUTA DE LA URL DE LA DIRECCIÓN
	char httpRequest = "GET ALGO\r\n\r\n";
	int writeResult = writeToServer(secureSocket, httpRequest);

	if(writeResult < strlen(httpRequest))
	{
		VER SI ESTE ERROR REALMENTE SE PUEDE DAR.
	}

	int maxDataLength = 10000;
	int totalBufferSpace = maxDataLength+1;
	char *readData=calloc(totalBufferSpace,1); //Take \0 into account since we are reading a string
	int readResultLength = 0;
	int totalReadBytes = 0;
	do
	{
		
		readResultLength = readFromServer(secureSocket, readData + totalReadBytes, maxDataLength);
		if(readResultLength < 0)
		{
			free(readData);
			return -1;	
		}
		
		totalReadBytes += readResultLength;

		if(totalReadBytes == maxDataLength)
		{
			totalBufferSpace += maxDataLength;
			readData = realloc(readData,totalBufferSpace);
			if(!readData)
			{
				return -1;
			}
		}
	}
	while(readResultLength < maxDataLength);

	closeSecureSocket(secureSocket);

	readData[totalReadBytes] = '\0'; //End the string with the null character. Remember we allocated
	

	printf("\n\n\n%s\n\n\n",);


	//char **returnedValues = scrapBTCAddresses(test,&resultLength,&resultCode,0);

	char **returnedValues = scrapBTCAddresses(readData,&resultLength,&resultCode,0);

    printf("\n\n\nPrinting results:\n\n\n");

    for(int i = 0; i < resultLength; i++)
    {

        
        printf("%s\n",returnedValues[i]);
    }

	free(readData);
	freeBTCAddressesArray(returnedValues, resultLength);
	    


}
