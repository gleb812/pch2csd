// Opens a new file

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

extern FILE *TempFile;
extern FILE *NewFile;

int OpenWrite(char TempFileName[50])
{
    char TEMP[5];
    const char iWave[]="iWave";
    char temp;
    const char Wave0[]="= giSin\n";
    const char Wave1[]="= giTri\n";
    const char Wave2[]="= giSaw\n";
    const char Wave3[]="= giSqr50\n";
    const char Wave4[]="= giSqr25\n";
    const char Wave5[]="= giSqr10\n";
    unsigned int WaveFormNumber;

    if((TempFile = fopen(TempFileName,"r")) == NULL)
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
	    int Count13=0;
	    while(true)
        {
            if(fread(&temp,1,1,TempFile) != 0)
            {
                TEMP[4]=TEMP[3];
                TEMP[3]=TEMP[2]
                TEMP[2]=TEMP[1]
                TEMP[1]=TEMP[0]
                TEMP[0]=temp;
                if(TEMP[0]==iWave[0])
                {
                    if(TEMP[1]==iWave[1])
                    {
                        if(TEMP[3]==iWave[2])
                        {
                            if(TEMP[4]==iWave[4])
                            {
                                if(TEMP[5]==iWave[5])
                                {
                                    if(fx)
                                    {
                                        WaveFormNumber=ModuleWaveFormListVA[];
                                    }
                                    else
                                    {
                                        WaveFormNumber=ModuleWaveFormListFX[];
                                    }
                                    switch(Color)
                                    {
                                        case 0:

                                        break;
                                        case 1:

                                        break;

                                        break;
                                        case 3:

                                        break;
                                        case 4:

                                        break;
                                        case 5:

                                        break;
                                        case 6:

                                        break;
                                        default:

                                    }
                                    fprintf(NewFile,)
                                }
                            }
                        }
                    }
                }
                if(Count13==3)
                {
                    fprintf(NewFile,)
                }
                fprintf(NewFile,"%c",temp);
            }
            else
            {
                fprintf(NewFile,"\n");
                break;
            }
        }
	}

	return 1;
}
