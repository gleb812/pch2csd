#include <stdio.h>
#include <stdbool.h>

#include <sys/types.h>
#include <dirent.h>

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
    char Name[4];
    unsigned int RulesTable[128][13];
    unsigned int K2AModulesIDCount;
    unsigned int K2AModulesIDTable[128];
    unsigned int counter;
    unsigned int tempnumber;
    unsigned int temp;

    char RulesK2AFileName[40] = "RulesK2A\\";
    FILE *RulesK2AFile;

    printf("*** READ ALL K2A RULES ***\n");

    K2AModulesIDCount = 0;

    // �������� ������ ������ Modules �� seludoM �� �������� "RulesK2A"

    DIR *dir = opendir("RulesK2A");
    if (dir) {
        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL) {
            i = 0;
            while (ent->d_name[i] != NULL) {
                Name[i] = ent->d_name[i];
                i++;
                if (i == 10) {
                    break;
                }
            }

            if (Name[0] != 0x2E) // if first symbol not "."
            {
                K2AModulesIDCount++;
                for (i = 0; i < 4; i++) {
                    if (Name[i] == 0x2E) // if symbol not "."
                    {
                        break;
                    } else {
                        K2AModulesIDTable[K2AModulesIDCount] =
                                K2AModulesIDTable[K2AModulesIDCount] * 10 + (unsigned int) (Name[i]) - 48;
                    }
                }


                //printf("%d\t",K2AModulesIDTable[K2AModulesIDCount]);


                tempnumber = K2AModulesIDTable[K2AModulesIDCount];
                counter = 1;

                while (true) {
                    tempnumber = (tempnumber - tempnumber % 10) / 10;
                    if (tempnumber == 0) {
                        break;
                    }
                    counter++;
                }

                for (i = 0; i < counter; i++) {

                    tempnumber = K2AModulesIDTable[K2AModulesIDCount];

                    for (j = 0; j < counter - i - 1; j++) {
                        tempnumber = tempnumber / 10;
                    }
                    tempnumber = tempnumber % 10;

                    RulesK2AFileName[9 + i] = (char) (48 + tempnumber);

                }

                //Module Name
                RulesK2AFileName[counter + 9] = 0x2e;
                RulesK2AFileName[counter + 10] = 0x74;
                RulesK2AFileName[counter + 11] = 0x78;
                RulesK2AFileName[counter + 12] = 0x74;
                RulesK2AFileName[counter + 13] = 0x0;

                if ((RulesK2AFile = fopen(RulesK2AFileName, "rb")) == NULL) {
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
