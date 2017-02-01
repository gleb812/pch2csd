#include <stdbool.h>

extern unsigned int IOTable[256][9];
extern unsigned int CableCounter;

int II2IO(void)
{
    unsigned int i,j;

    unsigned int findModulFrom;
    unsigned int findPortFrom;
    unsigned int findModulTo;
    unsigned int findPortTo;

    bool findFlag;

    for(i=0;i<CableCounter;i++)
    {
        if(IOTable[i][7]==0)
        {
            findFlag=false;

            findModulFrom=IOTable[i][2];
            findPortFrom=IOTable[i][3];

            findModulTo=IOTable[i][4];
            findPortTo=IOTable[i][5];

            for(j=0;j<CableCounter;j++)
            {
                if(IOTable[j][7]==1) // If cable type is input-to-output
                {
                    if(IOTable[j][4]==findModulTo) // module from is the same with module to
                    {
                        if(IOTable[j][5]==findPortTo) // Ports checking
                        {
                            if(IOTable[i][0]==IOTable[j][0])
                            {
                                findFlag=true;

                                IOTable[i][4]=IOTable[i][2]; // Old 'to-values' for module and port are substituted for its old 'from-values'
                                IOTable[i][5]=IOTable[i][3];

                                IOTable[i][2]=IOTable[j][2]; // Current 'from-values' for module and port are substituted for just found
                                IOTable[i][3]=IOTable[j][3];
                                //IOTable[i][1]=IOTable[j][1];
                                IOTable[i][7]=1;
                                break;
                            }
                        }
                    }

                    else
                    {
                        if(IOTable[j][4]==findModulFrom) // module to is the same with module from
                        {
                            if(IOTable[j][5]==findPortFrom) // Ports checking
                            {
                                findFlag=true;
                                IOTable[i][2]=IOTable[j][2]; // Current 'to-values' for module and port are substituted for just found
                                IOTable[i][3]=IOTable[j][3];
                                //IOTable[i][1]=IOTable[j][1];
                                IOTable[i][7]=1;
                                break;
                            }
                        }
                    }

                    if(findFlag)
                    {
                        break;
                    }
                }
            }
        }
    }

/*
    for(i=0;i<CableCounter;i++)
    {
        printf("Number = ");
        printf("%d ",IOTable[i][1]);
        printf("Type = ");
        printf("%d ",IOTable[i][7]);
        printf("MF = ");
        printf("%d ",IOTable[i][2]);
        printf("PF = ");
        printf("%d ",IOTable[i][3]);
        printf("MT = ");
        printf("%d ",IOTable[i][4]);
        printf("PT = ");
        printf("%d\n",IOTable[i][5]);
    }
*/

    return 1;
}

int II2IO4all(void)
{
    unsigned int i;
    printf("CableCounter = ");
    printf("%d\n",CableCounter);

    for(i=0;i<CableCounter;i++)
    {
        II2IO();
    }

    for(i=0;i<CableCounter;i++)
    {
        if(IOTable[i][7]==0)
        {
            printf("Patch has floating currents\n");
            break;
        }
    }

    return 1;
}
