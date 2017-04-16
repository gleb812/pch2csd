#include <stdio.h>
#include <stdbool.h>

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include "Config.h"

extern unsigned int CCa;
extern unsigned int aIOTable[256][6]; // signal cables list
//0-location (VA or FX); 1-cable ID; 2-module from; 3-port from;
//4-module to;  5-port to;

extern unsigned int ModuleTypeList[1024]; // Module Type list (modules with the same type are listed only once)
extern unsigned int ModuleTypeCount; // Module type counter

extern unsigned int ModuleCountVA, ModuleCountFX;
extern unsigned int ModuleCounter;
extern unsigned int ModuleListVA[1024];
extern unsigned int ModuleListFX[1024];

extern unsigned int ModuleIndexListVA[1024]; // VA Module Index list
extern unsigned int ModuleIndexListFX[1024]; // FX Module Index list

extern bool seludoMAlertVA[1024]; // VA flag of eludoM
extern bool seludoMAlertFX[1024]; // FX flag of eludoM

extern bool VAFXFlag;

int SearchK2AModules(void) {
    unsigned int i, j, c, Count, Rules, area;
    unsigned int RulesCount;
    unsigned int ModuleType;
    unsigned int ModuleTypeFlag;
    unsigned int RulesCounts[128];
    unsigned int RulesTable[128][13];
    unsigned int K2AModulesIDCount;
    unsigned int K2AModulesIDTable[128];
    unsigned int counter;
    unsigned int tempnumber;
    unsigned int temp;

    FILE *RulesK2AFile;

    printf("*** READ ALL K2A RULES ***\n");

    K2AModulesIDCount = 0;

    // �������� ������ ������ Modules �� seludoM �� �������� "RulesK2A"

    DIR *dir = opendir(NM_DirK2A);
    if (dir) {
        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] == '.') {
                continue;
            }
            K2AModulesIDCount++;

            i = 0;
            auto d_namlen = strlen(ent->d_name);
            for (i = 0; i < d_namlen; i++) {
                if (ent->d_name[i] == 0x2E) { // if symbol not "."
                    char fn[i];
                    strncpy(fn, ent->d_name, i);
                    char *end;
                    unsigned int id = (unsigned int) strtol(fn, &end, 10);
                    K2AModulesIDTable[K2AModulesIDCount] = id;
                    break;
                }
            }

            char k2aFile[1024];
            sprintf(k2aFile, "%s%d.txt", NM_DirK2A, K2AModulesIDTable[K2AModulesIDCount]);

            if ((RulesK2AFile = fopen(k2aFile, "rb")) == NULL) {
                //printf("N\n");
            } else {
                //printf("Y\n");

                RulesCount = 0;

                for (i = 0; i < 13; i++) {
                    if (fscanf(RulesK2AFile, "%d", &temp) == EOF) {
                        break;
                    } else {
                        RulesCount++;
                        RulesTable[K2AModulesIDCount][i] = temp;
                        RulesCounts[K2AModulesIDCount] = RulesCount;
                        //printf("Rule\t");
                        //printf("%d\n",temp);

                    }
                }

                fclose(RulesK2AFile);
            }
        }

        //printf("K2AModulesIDCount ");
        //printf("%d\n",K2AModulesIDCount);
        /*
        for(Count=0;Count<K2AModulesIDCount;Count++)
        {
            printf("RulesCounts ");
            printf("%d\n",RulesCounts[Count]);
        }
        */

        // ������ ���������� ������� �������� �������� ��������
        for (c = 0; c < CCa; c++) {
            /*
            printf("Number a-cable ");
            printf("%d\n",c);

            printf("Module To ");
            printf("%d\n",aIOTable[c][4]);

            printf("Port To ");
            printf("%d\n",aIOTable[c][5]);
            */

            temp = aIOTable[c][4];
            area = aIOTable[c][0];

            //printf("K2AModulesIDCount ");
            //printf("%d\n",K2AModulesIDCount);

            for (Count = 1; Count < K2AModulesIDCount + 1; Count++) {
                //printf("K2A Module ");
                //printf("%d\n",K2AModulesIDTable[Count]);

                if (area == 1) {
                    //printf("Module Type ");
                    //printf("%d\n",ModuleListVA[temp-1]);

                    if (ModuleListVA[temp - 1] == K2AModulesIDTable[Count]) {
                        /*
                        printf("Module_Finded\n");
                        printf("RulesCounts ");
                        printf("%d\n",RulesCounts[Count]);
                        */
                        for (Rules = 0; Rules < RulesCounts[Count] + 1; Rules++) {
                            //printf("Rule ");
                            //printf("%d\n",RulesTable[Count][Rules]);

                            if (aIOTable[c][5] == RulesTable[Count][Rules]) {
                                /*
                                printf("ModuleIndex #");
                                printf("%d",aIOTable[c][4]);
                                printf("seludoM\n");
                                */
                                seludoMAlertVA[temp - 1] = true;
                                break;
                            }
                        }

                    }

                }


            }

        }

    } else {
        fprintf(stderr, "Error opening directory\n");
    }

    closedir(dir);

    //VA

    for (ModuleCounter = 0; ModuleCounter < ModuleCountVA; ModuleCounter++) {
        if (seludoMAlertVA[ModuleCounter] == true) {
            ModuleListVA[ModuleCounter] = ModuleListVA[ModuleCounter] + 1000;

            ModuleType = ModuleListVA[ModuleCounter];
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

    //FX

    for (ModuleCounter = 0; ModuleCounter < ModuleCountFX; ModuleCounter++) {
        if (seludoMAlertFX[ModuleCounter] == true) {
            ModuleListFX[ModuleCounter] = ModuleListFX[ModuleCounter] + 1000;

            ModuleType = ModuleListFX[ModuleCounter];
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


    printf("*** Modules VA ***\n");
    //Head of table of Modules & seludoM
    printf("#\t");
    printf("Index\t");
    printf("Module\t");
    printf("eludoM\n");

    for (ModuleCounter = 0; ModuleCounter < ModuleCountVA; ModuleCounter++) {
        printf("%d\t", ModuleCounter);
        printf("%d\t", ModuleIndexListVA[ModuleCounter]);
        printf("%d\t", ModuleListVA[ModuleCounter]);
        if (seludoMAlertVA[ModuleCounter] == true) {
            printf("*");
        }
        printf("\n");
    }

    printf("*** Modules FX ***\n");
    //Head of table of Modules & seludoM
    printf("#\t");
    printf("Index\t");
    printf("Module\t");
    printf("eludoM\n");

    for (ModuleCounter = 0; ModuleCounter < ModuleCountFX; ModuleCounter++) {
        printf("%d\t", ModuleCounter);
        printf("%d\t", ModuleIndexListFX[ModuleCounter]);
        printf("%d\t", ModuleListFX[ModuleCounter]);
        if (seludoMAlertFX[ModuleCounter] == true) {
            printf("*");
        }
        printf("\n");
    }

    return 0;
};
