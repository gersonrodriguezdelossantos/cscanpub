#include "scraper.h"

#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

/*
1LuckyR1fFHEsXYyx5QK4UFzv3PEAepPMK
1P9RQEr2XeE3PEb44ZE35sfZRRW1JHU8qx
1dice7W2AicHosf5EL3GFDUVga7TgtPFn
1LuckyG4tMMZf64j6ea7JhCz7sDpk6vdcS
1G47mSr3oANXMafVrR8UC4pzV7FEAzo3r9
1dice1e6pdhLzzWQq7yMidf6j8eAg7pkY

1AX3CGMHys6J8vXPx8paxJ1He2t1ZVv9TY
1dice8EMZmqKvrGE4Qc9bUFf9PX3xaYDp


*/

char **scrapBTCAddresses(char *text, int *resultLength, int *resultCode, int debug)
{

    //Structure to store the compiled regex
    regex_t regex;
    int returnedCode;

    //Regex for Base58 and Bech32 addresses.
    char regexPattern[] = "[123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz]{26,35}|bc1[023456789acdefghjklmnpqrstuvwxyz]{6,87}";

    if(debug)
    {
        printf("Input string is \n%s.\nRegex pattern is %s\nString length is %lu\n",text,regexPattern,strlen(text));
    }

    //Compile the regex
    returnedCode = regcomp(&regex,regexPattern,REG_EXTENDED);
    if (returnedCode)
    {
        if(debug)
        {
            fprintf(stderr, "Cannot compile regex!!\n");
        }
        *resultCode = 1;
        return NULL;
    }
    else
    {
        if(debug)
        {
            printf("Regex compilation returned SUCCESS!!\n");
        }
    }

    //Define the array to return matched strings
    //The returned array of addresses is a length multiple of N*MAX_RESULTS
    const int MAX_RESULTS=3000;
    //The length of the result array
    int currentResultSize = MAX_RESULTS;
    //Allocate an array for MAX_RESULTS addresses
    char **returnedValues = calloc(MAX_RESULTS, sizeof(char *));
    //The current position of the result array to be used
    int returnedIndex=0;



    //Structure to store occurrences of the regex
    regmatch_t occurrences;
    //Current position of the string from where to start the search for a match
    char *currentPosition = text;
    do
    {
        //Find a substring that matches the regex
        returnedCode = regexec(&regex, currentPosition, 1, &occurrences, 0);

        //Handle error in regexec (could be a non-match or other errors)
        if(returnedCode)
        {
            if(debug)
            {
                printf("Returned Code is %i\n", returnedCode);
            }

            int errorMessageLength = 1000;
            char errorMessage[errorMessageLength];
            regerror(returnedCode, &regex, errorMessage, errorMessageLength);

            if(debug)
            {
                printf("ERROR MESSAGE IS %s\n", errorMessage);
            }

        }
        else
        {

            //If there is a match, then we copy the string
            if(debug)
            {
                printf("rm_so is %ld and rm_eo is %ld\n",occurrences.rm_so,occurrences.rm_eo);
                printf("Match goes from index %ld to index %ld\n", currentPosition-text+occurrences.rm_so, currentPosition-text+occurrences.rm_eo-1);
            }

            //If we have already filled the array with results, we need to enlarge it taking MAX_RESULTS as the block size
            if(returnedIndex == currentResultSize)
            {
                if(debug)
                {
                    printf("Current size is %i, reallocating\n",currentResultSize);
                }

                //Compute new size
                currentResultSize += MAX_RESULTS;
                //Allocate to the new greater size
                returnedValues = realloc(returnedValues,currentResultSize);

                //If realloc fails, then return NULL and exit with an error code
                if(!returnedValues)
                {
                    if(debug)
                    {
                        printf("Failed to reallocate memory!!!");
                    }
                    *resultCode = 2;
                    return NULL;
                }

                if(debug)
                {
                    printf("New size is %i\n",currentResultSize);
                }
            }

            //Adding the new address to the array

            //Length of BTC address
            int addressLength = occurrences.rm_eo-occurrences.rm_so;
            //Allocate memory for the address. Remember that we need to allocate one extra byt for \0!!!
            char *address = calloc(addressLength+1,sizeof(char));
            //Copy the string to the result array.
            memcpy(address,currentPosition+occurrences.rm_so,addressLength);
            //Point to the address in the result array
            returnedValues[returnedIndex]=address;
            //Increment the result length.
            returnedIndex++;



            if(debug)
            {
                //.*s indicates that we take a Number N=* of characters from the string. N is given by the second argument of the printf function and the string from where we take the N characters is the third argument.
                printf("Matched string is %.*s\n", (int) (occurrences.rm_eo - occurrences.rm_so), currentPosition+occurrences.rm_so);
                printf("Copied string is %s\n",address);
            }

        }

        if(debug)
        {
            printf("Current String is %s\n",currentPosition);
        }
        //Start the search from the next correct position
        if(!returnedCode)
        currentPosition += occurrences.rm_eo;

        if(debug)
        {
            printf("Current String is %s\n",currentPosition);
            printf("Current position is %li\n\n", currentPosition - text);
        }

    } while( !returnedCode && (currentPosition - text) < strlen(text) );

    //Free allocated memory for the compiled regex
    regfree(&regex);

    //Return SUCCESS (0) and the array of strings with the addresses
    *resultCode = 0;

    //Store the number of usable elements of the occurrences array and return the addresses
    *resultLength = returnedIndex;
    return returnedValues;
}

//Function to free the resources allocated to create a BTC Address array
void freeBTCAddressesArray(char **btcAddressArray, int arrayLength)
{
	if(!btcAddressArray)
	return;

	//Free each of the array components
	for(int i = 0; i < arrayLength ; i**)
	{
		free(btcAddressArray[i]);
	}

	//Free the array itself
	free(btcAddressArray);
}











