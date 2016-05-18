#include <stdio.h>
#include <stdbool.h>

extern FILE *TempFile;

extern unsigned int ModuleTypeCount;
extern unsigned int ModuleTypeList[1024];

extern char TempFileName[40];
extern char TempModuleMap[40];
extern char TempModuleIO[40];

int ModuleTableCheck(void)
{
    unsigned int i,j,k;
    unsigned int tempnumber;
    unsigned int counter;

    printf("*** Checking Library of Modules ***\n");
    printf("Number Modules in PatchFile\t%d\n",ModuleTypeCount);

    printf("--------------- *** ----------------\n");
    //printf("123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n");
    printf("#\t");
    printf("Modul\t");
    printf("csd\t");
    printf("Map\t");
    printf("IO2\n");

    for(k=0;k<ModuleTypeCount;k++)
    {
        tempnumber=ModuleTypeList[k];

        printf("%d\t",k);
        printf("%d\t",ModuleTypeList[k]);

        tempnumber=ModuleTypeList[k];

        counter=1;

        while(true)
        {
            tempnumber=(tempnumber-tempnumber%10)/10;
            if(tempnumber==0)
            {
                break;
            }
            counter++;
        }

        for(i=0;i<counter;i++)
        {
            tempnumber=ModuleTypeList[k];

            for(j=0;j<counter-i-1;j++)
            {
                tempnumber=tempnumber/10;
            }
            tempnumber=tempnumber%10;

            TempFileName[8+i]=(char)(48+tempnumber);
            TempModuleMap[5+i]=(char)(48+tempnumber);
            TempModuleIO[3+i]=(char)(48+tempnumber);

        }

        //Module Name
        TempFileName[counter+8]=0x2e;
        TempFileName[counter+9]=0x74;
        TempFileName[counter+10]=0x78;
        TempFileName[counter+11]=0x74;
        TempFileName[counter+12]=0x0;

        if((TempFile = fopen(TempFileName,"r")) == NULL)
        {
            printf("N\t");
        }
        else
        {
            printf("Y\t");
            fclose(TempFile);
        }

        //Mapping file name
        TempModuleMap[counter+5]=0x2e;
        TempModuleMap[counter+6]=0x74;
        TempModuleMap[counter+7]=0x78;
        TempModuleMap[counter+8]=0x74;
        TempModuleMap[counter+9]=0x0;

        if((TempFile = fopen(TempModuleMap,"r")) == NULL)
        {
            printf("N\t");
        }
        else
        {
            printf("Y\t");
            fclose(TempFile);
        }

        //формирование имени файла с определением входов и выходов
        TempModuleIO[counter+3]=0x2e;
        TempModuleIO[counter+4]=0x74;
        TempModuleIO[counter+5]=0x78;
        TempModuleIO[counter+6]=0x74;
        TempModuleIO[counter+7]=0x0;

        if((TempFile = fopen(TempModuleIO,"r")) == NULL)
        {
            printf("N\n");
        }
        else
        {
            printf("Y\n");
            fclose(TempFile);
        }

    }
    return 1;
}

