// Reads module IDs from patch file and creates a ModuleList / ModuleIndexList / ModuleTypeList

#include <stdio.h>
#include <stdbool.h>

extern FILE *ReadFile;

extern unsigned int MLposition;
extern unsigned int MLlength;

extern unsigned int ModuleCountVA, ModuleCountFX;

extern unsigned int ModuleListVA[1024];
extern unsigned int ModuleListFX[1024];

extern unsigned int ModuleIndexListVA[1024];
extern unsigned int ModuleIndexListFX[1024];

extern unsigned int ModuleTypeList[1024];
extern unsigned int ModuleTypeCount;

int ReadML(unsigned int position)
{
    unsigned char temp;
    unsigned int bytecounter=0;
    bool Data[100240];
    const unsigned int ModulLength=50;

    bool Modul[ModulLength];
    bool Tail[6];

    unsigned int i,j;
    bool location;
    bool LongModul;
    unsigned int ModuleCount;
    unsigned int ModuleType;
    unsigned int ModuleTypeFlag;
    unsigned int ModuleIndex;
    unsigned int HorizontalPosition;
    unsigned int VerticalPosition;
    unsigned int Color;
    unsigned int WaveForm;
    unsigned int Insert;
    unsigned int PadBits=0;

    bool vafx;

    fseek(ReadFile, position, SEEK_SET);

	while(true)
	{

	    if(fread(&temp,1,1,ReadFile)==0)
        {
            printf("module list not found\n");
            return 0;
        }
        else
        {
            if(temp==0x4a)
            {
                printf("*** module list (ML) ***\n");
                MLposition=position+bytecounter;
                break;
            }
        }
        bytecounter++;

	}

	printf("ML_position = ");
    printf("%d\n",MLposition);

	fread(&temp,1,1,ReadFile);
	MLlength=0xff*(unsigned int)temp;
	fread(&temp,1,1,ReadFile);
	MLlength=MLlength+(unsigned int)temp;

    printf("ML_Length = ");
	printf("%d\n",MLlength);

	for(i=0;i<MLlength*8;i++)
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
	for(i=0;i<MLlength*8;i++)
    {
        if(i%8==0)
        {
            fprintf(NewFile," ");
        }
        fprintf(NewFile,"%x",Data[i]);
    }
*/
	location=Data[1];

	printf("ML_Location = ");
	if(location==0)
	{
	    printf("FX Area\n");
	    vafx=false;
	}
	else
    {
        printf("Voice Area\n");
        vafx=true;
    }

    ModuleCount=0x80*Data[2]+0x40*Data[3]+0x20*Data[4]+0x10*Data[5]+0x08*Data[6]+0x04*Data[7]+0x02*Data[8]+Data[9];

    if(vafx)
    {
         ModuleCountVA=ModuleCount;
    }
    else
    {
         ModuleCountFX=ModuleCount;
    }

    printf("ML_Module_Count = ");
    printf("%d\n",ModuleCount);

     for(i=0;i<ModuleCount;i++)
    {

        for(j=0;j<ModulLength;j++)
        {
            Modul[j]=Data[10+i*ModulLength+j+PadBits];
        }

        for(j=0;j<6;j++)
        {
            Tail[j]=Data[10+i*ModulLength+ModulLength+j+PadBits];
        }

        printf("ML_Module_#");
        printf("%d\n",i);

        ModuleType=0x80*Modul[0]+0x40*Modul[1]+0x20*Modul[2]+0x10*Modul[3]+0x08*Modul[4]+0x04*Modul[5]+0x02*Modul[6]+Modul[7];

        if(vafx)
        {
            ModuleListVA[i]=ModuleType;
        }
        else
        {
            ModuleListFX[i]=ModuleType;
        }

        ModuleTypeFlag=0;
        for(j=0;j<1024;j++)
        {
            if(ModuleType!=ModuleTypeList[j])
            {
               ModuleTypeFlag++;
            }
        }

        if(ModuleTypeFlag==1024)
        {
            ModuleTypeList[ModuleTypeCount]=ModuleType;
            ModuleTypeCount++;
        }

        printf("ML_Module_Type = ");
        printf("%d\n",ModuleType);

        ModuleIndex=0x80*Modul[8]+0x40*Modul[9]+0x20*Modul[10]+0x10*Modul[11]+0x08*Modul[12]+0x04*Modul[13]+0x02*Modul[14]+Modul[15];

        if(vafx)
        {
            ModuleIndexListVA[i]=ModuleIndex;
        }
        else
        {
            ModuleIndexListFX[i]=ModuleIndex;
        }

        printf("ML_Module_Index = ");
        printf("%d\n",ModuleIndex);

        HorizontalPosition=0x40*Modul[16]+0x20*Modul[17]+0x10*Modul[18]+0x08*Modul[19]+0x04*Modul[20]+0x02*Modul[21]+Modul[22];

        printf("ML_Horizontal_position = ");
        printf("%d\n",HorizontalPosition);

        VerticalPosition=0x40*Modul[23]+0x20*Modul[24]+0x10*Modul[25]+0x08*Modul[26]+0x04*Modul[27]+0x02*Modul[28]+Modul[29];

        printf("ML_Vertical_position = ");
        printf("%d\n",VerticalPosition);

        Color=0x80*Modul[30]+0x40*Modul[31]+0x20*Modul[32]+0x10*Modul[33]+0x08*Modul[34]+0x04*Modul[35]+0x02*Modul[36]+Modul[37];

        printf("ML_Color = ");
        printf("%d\n",Color);

        //38-45 - empty

        Insert=0x08*Modul[46]+0x04*Modul[47]+0x02*Modul[48]+Modul[49];

        printf("ML_Insert = ");
        printf("%d\n",Insert);

        //50-51 - empty

        //UNUSUAL SWEDISH PADDING

        LongModul=false;

        if (Insert!=0)
        {
            LongModul=true;
            PadBits += 6*Insert;
        }
        else
        {
            LongModul=false;
        }
        // Somewhere here a wavetype is hidden from us

        if(LongModul)
        {
            WaveForm=0x08*Tail[2]+0x04*Tail[3]+0x02*Tail[4]+Tail[5];

            printf("ML_WaveForm = ");
            printf("%d\n",WaveForm);

            if(vafx)
            {
                ModuleWaveFormListVA[i]=WaveForm;
            }
            else
            {
                ModuleWaveFormListFX[i]=WaveForm;
            }
        }
        printf("\n");
    }

    printf("ML_Module_Type_List\n");
    for(i=0;i<ModuleTypeCount;i++)
    {
        printf("%d\n",ModuleTypeList[i]);
    }


    if(vafx)
    {
        printf("ML_Module_List_VA\n");
        for(i=0;i<ModuleCountVA;i++)
        {
            printf("%d\n",ModuleListVA[i]);
        }
    }
    else
    {
        printf("ML_Module_List_FX\n");
        for(i=0;i<ModuleCountFX;i++)
        {
            printf("%d\n",ModuleListFX[i]);
        }
    }

	return 1;

}
