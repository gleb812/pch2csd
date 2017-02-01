// Cable list sorting

// While reading the patch file all cables are placed in one IOTable list
// Two separate lists are formed: one for control cables and one for signals,
// because CSound zak-space has two different spaces (a and k)

extern unsigned int CCa,CCk; // a and k cables counters
extern unsigned int CableCounter; // common counter
extern unsigned int aIOTable[256][6]; // signal cables list
                                //0-location (VA or FX); 1-cable ID; 2-module from; 3-port from;
                                //4-module to;  5-port to;
extern unsigned int kIOTable[256][6]; // control cables list
                                //0-location (VA or FX); 1-cable ID; 2-module from; 3-port from;
                                //4-module to;  5-port to;
extern unsigned int IOTable[256][9]; // all cables list
extern unsigned int kzakNumber, azakNumber; // zakspace counters

int CableSort(void)
{
    unsigned int i,j;
    unsigned int atemp,ktemp;
    unsigned int FXShift;

    CCa=0;
    CCk=0;

    atemp=0;
    ktemp=0;

    for(i=0;i<CableCounter;i++)
    {
        if(IOTable[i][8]==1) //a-cable
        {
            aIOTable[CCa][0]=IOTable[i][0];

            if(IOTable[i][0]==1)
            {
                if(atemp<IOTable[i][1])
                {
                    atemp=IOTable[i][1];
                }
                aIOTable[CCa][1]=CCa;
            }
            else
            {
                aIOTable[CCa][1]=CCa-atemp-1;
            }
            aIOTable[CCa][2]=IOTable[i][2];
            aIOTable[CCa][3]=IOTable[i][3];
            aIOTable[CCa][4]=IOTable[i][4];
            aIOTable[CCa][5]=IOTable[i][5];
            CCa++;
        }
        if(IOTable[i][8]==0) //k-cable
        {
            kIOTable[CCk][0]=IOTable[i][0];

            if(IOTable[i][0]==1)
            {
                if(ktemp<IOTable[i][1])
                {
                    ktemp=IOTable[i][1];
                }
                kIOTable[CCk][1]=CCk;
            }
            else
            {
                kIOTable[CCk][1]=CCk-ktemp-1;
            }
            kIOTable[CCk][2]=IOTable[i][2];
            kIOTable[CCk][3]=IOTable[i][3];
            kIOTable[CCk][4]=IOTable[i][4];
            kIOTable[CCk][5]=IOTable[i][5];
            CCk++;
        }
    }

    // Due to differences in Nord and Csound Zak patching description
    // We need to perform a mapping
    // If different cables have the same source, this will be the same cable

    // for k-cables
    for(i=0;i<CCk;i++)
    {
        for(j=i;j<CCk;j++)
        {
            if(i!=j)
            {
                if(kIOTable[i][2]==kIOTable[j][2])
                {
                    if(kIOTable[i][3]==kIOTable[j][3])
                    {
                        kIOTable[j][1]=kIOTable[i][1];
                    }
                }
            }
        }
    }

    // for a-cables
    for(i=0;i<CCa;i++)
    {
        for(j=i;j<CCa;j++)
        {
            if(i!=j)
            {
                if(aIOTable[i][2]==aIOTable[j][2])
                {
                    if(aIOTable[i][3]==aIOTable[j][3])
                    {
                        aIOTable[j][1]=aIOTable[i][1];
                    }
                }
            }
        }
    }

    // In Csound zakspace there is one patching field,
    // comparing to FX and VA patching fields of Nord
    // So we shift ID valules of FX field
    FXShift=0;
    for(i=0;i<CCk;i++)
    {
        if(kIOTable[i][0]==1)
        {
            if(FXShift<kIOTable[i][1])
            {
                FXShift=kIOTable[i][1];
            }
        }
    }
    FXShift++;
    for(i=0;i<CCk;i++)
    {
        if(kIOTable[i][0]==0)
        {
            kIOTable[i][1]+=FXShift;
        }
    }

    FXShift=0;
    for(i=0;i<CCa;i++)
    {
        if(aIOTable[i][0]==1)
        {
            if(FXShift<aIOTable[i][1])
            {
                FXShift=aIOTable[i][1];
            }
        }
    }
    FXShift++;
    for(i=0;i<CCa;i++)
    {
        if(aIOTable[i][0]==0)
        {
            aIOTable[i][1]+=FXShift;
        }
    }

    // patching fields size computation
    //printf("k-Cables\n");
    for(i=0;i<CCk;i++)
    {
        if(kzakNumber<kIOTable[i][1])
        {
            kzakNumber+=kIOTable[i][1];
        }
        /*
        printf("Location = ");
        printf("%d ",kIOTable[i][0]);
        printf("Number = ");
        printf("%d ",kIOTable[i][1]);
        printf("MF = ");
        printf("%d ",kIOTable[i][2]);
        printf("PF = ");
        printf("%d ",kIOTable[i][3]);
        printf("MT = ");
        printf("%d ",kIOTable[i][4]);
        printf("PT = ");
        printf("%d\n",kIOTable[i][5]);
        */
    }

    //printf("a-Cables\n");
    for(i=0;i<CCa;i++)
    {
        if(azakNumber<aIOTable[i][1])
        {
            azakNumber+=aIOTable[i][1];
        }

        printf("Location = ");
        printf("%d ",aIOTable[i][0]);
        printf("Number = ");
        printf("%d ",aIOTable[i][1]);
        printf("MF = ");
        printf("%d ",aIOTable[i][2]);
        printf("PF = ");
        printf("%d ",aIOTable[i][3]);
        printf("MT = ");
        printf("%d ",aIOTable[i][4]);
        printf("PT = ");
        printf("%d\n",aIOTable[i][5]);

    }

    return 1;
}
