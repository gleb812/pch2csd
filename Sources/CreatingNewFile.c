#include <stdio.h>
#include <stdlib.h>
#include "Util.c"

extern FILE *NewFile;

// function that creates a new file
int CreatingNewFile(char NewFileName[50])
{
	char *f = PreparePathString(NewFileName);
    if((NewFile = fopen(f,"w+b")) == NULL)
	{
		printf("Error - file not created!\n");
		return 0;
	}
	else
	{
		printf("file was created!\n");
	}
	free(f);
	return 1;
}
