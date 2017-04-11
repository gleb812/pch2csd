// Function creates a ***** separator in an output csd file

#include <stdio.h>

extern FILE *NewFile;

int NextField(void)
{
    int i;
    fprintf(NewFile,"\n");
    fprintf(NewFile,";");
    for(i=0;i<30;i++)
    {
        fprintf(NewFile,"*");
    }
    fprintf(NewFile,"\n");

    return 1;
}
