// Function generates the list of used Instrs
// according to ModuleTypeList.
// The Instr descriptions are in /modules
// They are copied in csd file, according to OWModule
#include <stdio.h>

extern FILE *NewFile;

extern unsigned int ModuleTypeCount;
extern unsigned int ModuleTypeList[1024];

int GenInstruments(void) {
    unsigned int i;
    printf("Generating instruments list\n");
    fprintf(NewFile, "; Opcode Definitions\n");

    //printf("%d\n",ModuleTypeCount);
    //printf("Module_Type_List\n");
    for (i = 0; i < ModuleTypeCount; i++) {
        //printf("#%d\n",ModuleTypeList[i]);
        OWModule(ModuleTypeList[i]);
        fprintf(NewFile, "\n");
    }
    return 1;
}
