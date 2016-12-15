// Function reads the parameters of a module

#include <stdio.h>
#include <stdbool.h>

extern FILE *ReadFile;

extern unsigned int MPposition;
extern unsigned int MPlength;

extern unsigned int ParameterCountersVA[128];
extern unsigned int ParameterCountersFX[128];

extern unsigned int ParametersVA[128][64];
extern unsigned int ParametersFX[128][64];

extern unsigned int HiddenParametersVA[128]; //Table with VA Module hidden parameter value (Module number 0-127)
extern unsigned int HiddenParametersFX[128]; //Table with FX Module hidden parameter value (Module number 0-127)

extern bool HiddenFlagVA[128]; //Flag VA if Module has the hidden parameter (Module number 0-127)
extern bool HiddenFlagFX[128]; //Flag FX if Module has the hidden parameter (Module number 0-127)

int ReadMP(unsigned int position) //Reading Module Parameters
{
    unsigned char temp;
    unsigned int bytecounter=0;
    bool Data[102400];
    unsigned int i,j,k,g,n;
    bool location;

    unsigned int ModuleCount;
    unsigned int tempposition;
    const unsigned int ModuleHeadLength=15;
    bool ModuleHead[ModuleHeadLength];
    const unsigned int ModuleParameterLength=15;
    bool ModuleParameter[ModuleParameterLength];
    unsigned int ModuleIndex;
    unsigned int ParameterCount;
    unsigned int Variation;
    unsigned int Value;

    fseek(ReadFile, position, SEEK_SET);

	while(true)
	{

	    if(fread(&temp,1,1,ReadFile)==0)
        {
            printf("Module Parameters not found\n");
            return 0;
        }
        else
        {
            if(temp==0x4d)
            {
                printf("*** Module Parameters (MP) ***\n");
                MPposition=position+bytecounter;
                break;
            }
        }
        bytecounter++;

	}

	printf("MP_position = ");
    printf("%d\n",MPposition);

	fread(&temp,1,1,ReadFile);
	MPlength=0xff*(unsigned int)temp;
	fread(&temp,1,1,ReadFile);
	MPlength=MPlength+(unsigned int)temp;

    printf("MP_Length = ");
	printf("%d\n",MPlength);

    for(i=0;i<MPlength*8;i++)
	{
	    if((i%8)==0)
        {
            fread(&temp,1,1,ReadFile);

            for(j=0;j<8;j++)
            {
                if(((temp>>(8-(j+1)))&0x01)>0)
                {
                    Data[i+j]=true;
                }
                else
                {
                    Data[i+j]=false;
                }
            }
        }
	}

    /*
    for(i=0;i<MPlength*8;i++)
    {
        if(i%8==0)
        {
            fprintf(UpCodeFile," ");
        }
        fprintf(NewFile,"%x",Data[i]);
    }
    */

	location=Data[1];

	printf("MP_Location = ");
	if(location==0)
	{
	    printf("FX Area\n");
	}
	else
    {
        printf("Voice Area\n");
    }

    ModuleCount=0x80*Data[2]+0x40*Data[3]+0x20*Data[4]+0x10*Data[5]+0x08*Data[6]+0x04*Data[7]+0x02*Data[8]+Data[9];

    printf("MP_Module_Count = ");
    printf("%d\n",ModuleCount);

    tempposition=18;

    for(i=0;i<ModuleCount;i++)
    {
        for(j=0;j<ModuleHeadLength;j++)
        {
            ModuleHead[j]=Data[tempposition];
            tempposition++;
        }
        ModuleIndex=0x80*ModuleHead[0]+0x40*ModuleHead[1]+0x20*ModuleHead[2]+0x10*ModuleHead[3]+0x08*ModuleHead[4]+0x04*ModuleHead[5]+0x02*ModuleHead[6]+ModuleHead[7];
        printf("MP_Module_Index = ");
        printf("%d\n",ModuleIndex);
        //ParameterCount=0x80*ModuleHead[8]+0x40*ModuleHead[9]+0x20*ModuleHead[10]+0x10*ModuleHead[11]+0x08*ModuleHead[12]+0x04*ModuleHead[13]+0x02*ModuleHead[14]+ModuleHead[15];
        ParameterCount=0x40*ModuleHead[8]+0x20*ModuleHead[9]+0x10*ModuleHead[10]+0x08*ModuleHead[11]+0x04*ModuleHead[12]+0x02*ModuleHead[13]+ModuleHead[14];
        printf("MP_Module_Parameter_Count = ");
        printf("%d\n",ParameterCount);
        if(location)
        {
            ParameterCountersVA[ModuleIndex-1]=ParameterCount;
        }
        else
        {
            ParameterCountersFX[ModuleIndex-1]=ParameterCount;
        }

        for(k=0;k<9;k++)
        {
            for(g=0;g<8;g++)
            {
                ModuleParameter[g]=Data[tempposition];
                tempposition++;
            }

            Variation=0x80*ModuleParameter[0]+0x40*ModuleParameter[1]+0x20*ModuleParameter[2]+0x10*ModuleParameter[3]+0x08*ModuleParameter[4]+0x04*ModuleParameter[5]+0x02*ModuleParameter[6]+ModuleParameter[7];
            printf("MP_Variation = ");
            printf("%d\n",Variation);

            for(g=0;g<ParameterCount;g++)
            {
                for(n=0;n<7;n++)
                {
                    ModuleParameter[n]=Data[tempposition];
                    tempposition++;
                }
                Value=0x40*ModuleParameter[0]+0x20*ModuleParameter[1]+0x10*ModuleParameter[2]+0x08*ModuleParameter[3]+0x04*ModuleParameter[4]+0x02*ModuleParameter[5]+ModuleParameter[6];
                printf("MP_Value = ");
                printf("%d\n",Value);

                if(k==0)
                {
                    if(location)
                    {
                        ParametersVA[ModuleIndex-1][g]=Value;
                    }
                    else
                    {
                        ParametersFX[ModuleIndex-1][g]=Value;
                    }

                }

            }

            // Add parameter value from ModuleList
            if(location)
            {
                if(HiddenFlagVA[ModuleIndex-1])
                {
                    ParameterCountersVA[ModuleIndex-1]=ParameterCount+1;
                    ParametersVA[ModuleIndex-1][ParameterCount]=HiddenParametersVA[ModuleIndex-1];
                    printf("MP_Hidden_Value = ");
                    printf("%d\n",HiddenParametersVA[ModuleIndex-1]);
                }
            }
            else
            {
                if(HiddenFlagFX[ModuleIndex-1])
                {
                    ParameterCountersFX[ModuleIndex-1]=ParameterCount+1;
                    ParametersFX[ModuleIndex-1][ParameterCount
                    ]=HiddenParametersFX[ModuleIndex-1];
                    printf("MP_Hidden_Value = ");
                    printf("%d\n",HiddenParametersFX[ModuleIndex-1]);
                }
            }

        }

    }

	return 1;

}
