// Opens a new file

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "Util.c"

extern FILE *TempFile;
extern FILE *NewFile;

int OpenWrite(char TempFileName[50])
{
    char *TempFileName_ = PreparePathString(TempFileName);

    char temp;
    if((TempFile = fopen(TempFileName_,"r")) == NULL)
	{
	    /*
		printf("Error - ");
		printf(TempFileName);
		printf(" not opened!\n");
		*/
		return 0;
	}
	else
	{
	    while(true)
        {
            if(fread(&temp,1,1,TempFile) != 0)
            {
                fprintf(NewFile,"%c",temp);
            }
            else
            {
                fprintf(NewFile,"\n");
                break;
            }
        }
	}

    free(TempFileName_);

	return 1;
}
