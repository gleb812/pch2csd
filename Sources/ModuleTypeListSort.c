extern unsigned int ModuleCountVA; // VA field module counter
extern unsigned int ModuleCountFX; // FX field module counter

extern unsigned int ModuleListVA[1024]; // VA module list
extern unsigned int ModuleListFX[1024]; // FX module list

extern unsigned int ModuleTypeList[1024]; // Module Type list (modules with the same type are listed only once)
extern unsigned int ModuleTypeCount; // Module type counter

int ModuleTypeListSort(void) {
    unsigned int i, j;
    unsigned int ModuleType;
    unsigned int ModuleTypeFlag;

    ModuleTypeCount = 0;

    for (i = 0; i < 1024; i++) {
        ModuleTypeList[i] = 0;
    }
    ModuleTypeList[1024];

    //VA
    for (i = 0; i < ModuleCountVA; i++) {
        ModuleType = ModuleListVA[i];
        ModuleTypeFlag = 0;

        for (j = 0; j < 1024; j++) {
            if (ModuleType != ModuleTypeList[j]) {
                ModuleTypeFlag++;
            }
        }

        if (ModuleTypeFlag == 1024) {
            ModuleTypeList[ModuleTypeCount] = ModuleType;
            ModuleTypeCount++;
        }
    }
    //FX
    for (i = 0; i < ModuleCountFX; i++) {
        ModuleType = ModuleListFX[i];
        ModuleTypeFlag = 0;

        for (j = 0; j < 1024; j++) {
            if (ModuleType != ModuleTypeList[j]) {
                ModuleTypeFlag++;
            }
        }

        if (ModuleTypeFlag == 1024) {
            ModuleTypeList[ModuleTypeCount] = ModuleType;
            ModuleTypeCount++;
        }
    }
}
