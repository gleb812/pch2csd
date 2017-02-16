// Zakinit initialization
// We use azakNumber and kzakNumber from CableSort
// and create patching field of corresponding sizes

#include <stdio.h>

extern unsigned int kzakNumber, azakNumber;
extern unsigned int OtherCableCount;
extern FILE *NewFile;

int GenZakInit(void)
{
    printf("Generation zakinit\n");

    printf("Sound_Cable_Count = ");
    printf("%d\n",azakNumber);
    printf("Control_Cable_Count = ");
    printf("%d\n",kzakNumber);
    printf("Other_Cable_Count = ");
    printf("%d\n",OtherCableCount);

    fprintf(NewFile,"; Initialize the ZAK space\n");
    fprintf(NewFile,"zakinit ");
    fprintf(NewFile,"%d",azakNumber+2);
    fprintf(NewFile,", ");
    fprintf(NewFile,"%d\n",kzakNumber+2);

    if(OtherCableCount>0)
    {
        printf("DANGER - Exotic Colors of Cables\n");
    }

    return 1;

}
