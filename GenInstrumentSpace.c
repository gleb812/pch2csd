// Function initializes GenInstrumentContent routine for each of two module lists (VA and FX)
#include <stdio.h>
#include <stdbool.h>

extern FILE *NewFile;

extern unsigned int ModuleCountVA, ModuleCountFX;
extern unsigned int ModuleCounter;
extern unsigned int ModuleListVA[1024];
extern unsigned int ModuleListFX[1024];

extern bool VAFXFlag;

int GenInstrumentSpace(void)
{
    printf("Generating instrument \n");
    fprintf(NewFile,"instr 1\n");
    //printf("Modules_List_VA = ");
    //printf("%d\n",ModuleCountVA);
    VAFXFlag=true;
    for(ModuleCounter=0;ModuleCounter<ModuleCountVA;ModuleCounter++)
    {
        //printf("#%d\n",ModuleListVA[ModuleCounter]);
        GenInstrumentContent(ModuleListVA[ModuleCounter]);
        fprintf(NewFile,"\n");
    }
    fprintf(NewFile,"endin\n");

    fprintf(NewFile,"instr 2\n");
    //printf("Modules_List_FX = ");
    //printf("%d\n",ModuleCountFX);
    VAFXFlag=false;
    for(ModuleCounter=0;ModuleCounter<ModuleCountFX;ModuleCounter++)
    {
        //printf("#%d\n",ModuleListFX[ModuleCounter]);
        GenInstrumentContent(ModuleListFX[ModuleCounter]);
        fprintf(NewFile,"\n");
    }
    fprintf(NewFile,"endin\n");
    return 1;
}
