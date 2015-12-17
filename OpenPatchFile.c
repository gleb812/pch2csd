// function that opens a NORD patch file

#include <stdio.h>

extern FILE *ReadFile;


int OpenPatchFile(char PatchFileName[20])
{
    if((ReadFile = fopen(PatchFileName,"r+b")) == NULL)
	{
		printf("Error - file not opened!\n");
		return 0;
	}
	else
	{
		printf("file was opened!\n");
	}
	return 1;
}
