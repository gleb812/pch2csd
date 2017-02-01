// function that opens a NORD patch file

#include <stdio.h>
#include <stdlib.h>
#include "Util.c"

extern FILE *ReadFile;

int OpenPatchFile(char PatchFileName[20])
{
    char *PatchFileName_ = PreparePathString(PatchFileName);

    if((ReadFile = fopen(PatchFileName_,"r+b")) == NULL)
	{
		printf("Error - file not opened!\n");
        return 0;
	}
    free(PatchFileName_);
	return 1;
}
