// function that opens a NORD patch file

#include <stdio.h>

extern FILE *ReadFile;
extern FILE *RecentFile;
extern char RecentFileName[50];

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
		if((RecentFile = fopen(RecentFileName,"w+b")) == NULL)
        {
            printf("Error with saving patch-file to recent file!\n");
        }
        else
        {
            fputs(PatchFileName, RecentFile);
            fclose(RecentFile);
        }
	}
	return 1;
}
