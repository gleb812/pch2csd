// The most mystical and obscure part of a code.
// We just read and skip some Mystery Objects

#include <stdio.h>
#include <stdbool.h>

extern FILE *ReadFile;

extern unsigned int MOposition;
extern unsigned int MOlength;

int ReadMO(unsigned int position) //Reading Mystery Objects
{
    unsigned char temp;
    unsigned int bytecounter=0;

    fseek(ReadFile, position, SEEK_SET);

	while(true)
	{

	    if(fread(&temp,1,1,ReadFile)==0)
        {
            printf("mystery object not found\n");
            return 0;
        }
        else
        {
            if(temp==0x69)
            {
                printf("*** Mystery Object (MO) ***\n");
                MOposition=position+bytecounter;
                break;
            }
        }
        bytecounter++;

	}

	printf("MO_position = ");
    printf("%d\n",MOposition);

	fread(&temp,1,1,ReadFile);
	MOlength=0xff*(unsigned int)temp;
	fread(&temp,1,1,ReadFile);
	MOlength=MOlength+(unsigned int)temp;

    printf("MO_Length = ");
	printf("%d\n",MOlength);

	return 1;

}
