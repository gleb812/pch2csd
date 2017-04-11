// Function should read the patch settings
// We skip it in this version

#include <stdio.h>
#include <stdbool.h>

extern FILE *ReadFile;

extern unsigned int PSposition;
extern unsigned int PSlength;

int ReadPS(unsigned int position) //Reading Patch Settings
{
    unsigned char temp;
    unsigned int bytecounter=0;

    fseek(ReadFile, position, SEEK_SET);

	while(true)
	{

	    if(fread(&temp,1,1,ReadFile)==0)
        {
            printf("Patch Settings not found\n");
            return 0;
        }
        else
        {
            if(temp==0x4d)
            {
                printf("*** Patch Settings (PS) ***\n");
                PSposition=position+bytecounter;
                break;
            }
        }
        bytecounter++;

	}

	printf("PS_position = ");
    printf("%d\n",PSposition);

	fread(&temp,1,1,ReadFile);
	PSlength=0xff*(unsigned int)temp;
	fread(&temp,1,1,ReadFile);
	PSlength=PSlength+(unsigned int)temp;

    printf("PS_Length = ");
	printf("%d\n",PSlength);

	return 1;

}
