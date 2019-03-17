#include <stdio.h>

#include "scraper.h"

int main(int argc, char ** argv)
{



    char test[]="1BvBMSEYstWetqTFn5Au4m4GFg7xJaNVN2,3J98t1WpEZ73CNmQviecrnyiWrnqRhWNLy,bc1qar0srrr7xfkvy5l643lydnw9re59gtzzwf5mdq,,d";

    int resultLength = 0;
    int resultCode = 0;
    char **returnedValues = scrapBTCAddresses(test,&resultLength,&resultCode,0);

 

    printf("Printing results:\n\n\n");

    for(int i = 0; i < resultLength; i++)
    {

        
        printf("%s\n",returnedValues[i]);
    }
    


}