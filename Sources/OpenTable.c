// Function opens a parameter map table
#include <stdio.h>
#include <stdlib.h>
#include "Util.c"

extern FILE *TempFile;
extern float Tables[128][128];

int OpenTable(char TempFileName[20],unsigned int TableID)
{
    char *TempFileName_ = PreparePathString(TempFileName);
    float value;
    unsigned int i;

    if((TempFile = fopen(TempFileName_,"r")) == NULL)
	{
		printf("Error - ");
		printf(TempFileName);
		printf(" not opened!\n");
		return 0;
	}
	else
	{
	    printf(TempFileName);
	    printf("\n");
	    for(i=0;i<128;i++)
        {
            if(fscanf(TempFile, "%f", &value)==EOF)
            {
                break;
            }
            else
            {
                Tables[i][TableID]=value;
                //printf("%f\n",value);
            }

        }
	}

    free(TempFileName_);

    return 1;
}
