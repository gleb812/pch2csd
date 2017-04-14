#include <stdio.h>
#include <stdbool.h>
#include "Config.h"

extern FILE *TempFile;

extern unsigned int ModuleTypeCount;
extern unsigned int ModuleTypeList[1024];

int ModuleTableCheck(void) {
    unsigned int k;
    unsigned char tempSymbol;
    bool NameFlag = false;
    unsigned int NameCount = 0;

    printf("*** Checking Library of Modules ***\n");
    if ((TempFile = fopen(NM_FileModID2Name, "rb")) == NULL) {
        printf("Table with names of modules not found");
        NameFlag = false;
    } else {
        NameFlag = true;
    }
    printf("Number Modules in PatchFile\t%d\n", ModuleTypeCount);

    printf("--------------- *** ----------------\n");
    printf("#\t");
    printf("Module  ");
    printf("csd\t");
    printf("Map\t");
    printf("IO\t");
    printf("Name\n");

    for (k = 0; k < ModuleTypeCount; k++) {

        printf("%d\t", k);
        printf("%d\t", ModuleTypeList[k]);

        char moduleFile[1024];
        char mapFile[1024];
        char ioFile[1024];

        sprintf(moduleFile, "%s%d.txt", NM_DirModules, ModuleTypeList[k]);
        sprintf(mapFile, "%s%d.txt", NM_DirMaps, ModuleTypeList[k]);
        sprintf(ioFile, "%s%d.txt", NM_DirIO, ModuleTypeList[k]);

        if ((TempFile = fopen(moduleFile, "rb")) == NULL) {
            printf("N\t");
        } else {
            printf("Y\t");
            fclose(TempFile);
        }

        if ((TempFile = fopen(mapFile, "rb")) == NULL) {
            printf("N\t");
        } else {
            printf("Y\t");
            fclose(TempFile);
        }

        if ((TempFile = fopen(ioFile, "rb")) == NULL) {
            printf("N\t");
        } else {
            printf("Y\t");
            fclose(TempFile);
        }

        if (NameFlag == true) {
            TempFile = fopen(NM_FileModID2Name, "rb");
            NameCount = 0;

            if (ModuleTypeList[k] != 1) {
                while (true) {
                    if (fread(&tempSymbol, 1, 1, TempFile) == 0x00) {
                        break;
                    } else {
                        if (tempSymbol == 0x0d) {
                            NameCount++;
                            if (NameCount == ModuleTypeList[k] - 1) {
                                break;
                            }
                        }
                    }
                }
            }

            while (true) {
                if (fread(&tempSymbol, 1, 1, TempFile) == 0) {
                    break;
                } else {
                    if (tempSymbol == 0x09) {
                        break;
                    }

                }
            }

            while (true) {
                if (fread(&tempSymbol, 1, 1, TempFile) == 0) {
                    break;
                } else {
                    printf("%c", tempSymbol);
                    if (tempSymbol == 0x0d) {
                        break;
                    }

                }
            }

            fclose(TempFile);
        }

        printf("\n");

    }
    return 1;
}
