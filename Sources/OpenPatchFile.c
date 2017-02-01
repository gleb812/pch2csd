// function that opens a NORD patch file

#include <stdio.h>
#include <stdlib.h>
#include "Util.c"

extern FILE *ReadFile;
extern FILE *RecentFile;
extern char RecentFileName[50];

int OpenPatchFile(char PatchFileName[20])
{
    char *PatchFileName_ = PreparePathString(PatchFileName);
    char *RecentFileName_ = PreparePathString(RecentFileName);

    if((ReadFile = fopen(PatchFileName_,"r+b")) == NULL)
	{
		printf("Error - file not opened!\n");
        return 0;
	}
	else
	{
		printf("file was opened!\n");
		if((RecentFile = fopen(RecentFileName_,"w+b")) == NULL)
        {
            printf("Error with saving patch-file to recent file!\n");
        }
        else
        {
            fputs(PatchFileName, RecentFile);
            fclose(RecentFile);
        }
	}
    free(PatchFileName_);
    free(RecentFileName_);
	return 1;
}
