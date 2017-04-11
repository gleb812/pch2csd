#include <stdio.h>
#include <stdbool.h>

extern FILE *TempFile;

extern unsigned int ModuleTypeCount;
extern unsigned int ModuleTypeList[1024];

extern char TempFileName[40];
extern char TempModuleMap[40];
extern char TempModuleIO[40];
extern char ModuleNamesTable[40];

int ModuleTableCheck(void) {
    unsigned int i, j, k;
    unsigned int maptempnumber;
    unsigned int tempnumber;
    unsigned int counter;
    //char Name[20];
    unsigned char tempSymbol;
    bool NameFlag = false;
    unsigned int NameCount = 0;

    printf("*** Checking Library of Modules ***\n");
    if ((TempFile = fopen(ModuleNamesTable, "rb")) == NULL) {
        printf("Table with names of modules not found");
        NameFlag = false;
    } else {
        NameFlag = true;
    }
    printf("Number Modules in PatchFile\t%d\n", ModuleTypeCount);

    printf("--------------- *** ----------------\n");
    //printf("123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n");
    printf("#\t");
    printf("Module  ");
    printf("csd\t");
    printf("Map\t");
    printf("IO\t");
    printf("Name\n");

    for (k = 0; k < ModuleTypeCount; k++) {
        tempnumber = ModuleTypeList[k];

        printf("%d\t", k);
        printf("%d\t", ModuleTypeList[k]);

        if (ModuleTypeList[k] > 1000) {
            maptempnumber = ModuleTypeList[k] - 1000;
        } else {
            maptempnumber = ModuleTypeList[k];
        }

        tempnumber = maptempnumber;
        counter = 1;

        while (true) {
            tempnumber = (tempnumber - tempnumber % 10) / 10;
            if (tempnumber == 0) {
                break;
            }
            counter++;
        }

        for (i = 0; i < counter; i++) {
            tempnumber = maptempnumber;

            for (j = 0; j < counter - i - 1; j++) {
                tempnumber = tempnumber / 10;
            }
            tempnumber = tempnumber % 10;

            TempModuleMap[5 + i] = (char) (48 + tempnumber);

        }

        tempnumber = ModuleTypeList[k];

        counter = 1;

        while (true) {
            tempnumber = (tempnumber - tempnumber % 10) / 10;
            if (tempnumber == 0) {
                break;
            }
            counter++;
        }


        for (i = 0; i < counter; i++) {
            tempnumber = ModuleTypeList[k];

            for (j = 0; j < counter - i - 1; j++) {
                tempnumber = tempnumber / 10;
            }
            tempnumber = tempnumber % 10;

            TempFileName[8 + i] = (char) (48 + tempnumber);
            TempModuleIO[3 + i] = (char) (48 + tempnumber);

        }



        //Module Name
        TempFileName[counter + 8] = 0x2e;
        TempFileName[counter + 9] = 0x74;
        TempFileName[counter + 10] = 0x78;
        TempFileName[counter + 11] = 0x74;
        TempFileName[counter + 12] = 0x0;

        if ((TempFile = fopen(TempFileName, "rb")) == NULL) {
            printf("N\t");
        } else {
            printf("Y\t");
            fclose(TempFile);
        }

        //Mapping file name
        if (ModuleTypeList[k] > 1000) {
            TempModuleMap[counter + 4] = 0x2e;
            TempModuleMap[counter + 5] = 0x74;
            TempModuleMap[counter + 6] = 0x78;
            TempModuleMap[counter + 7] = 0x74;
            TempModuleMap[counter + 8] = 0x0;
        } else {
            TempModuleMap[counter + 5] = 0x2e;
            TempModuleMap[counter + 6] = 0x74;
            TempModuleMap[counter + 7] = 0x78;
            TempModuleMap[counter + 8] = 0x74;
            TempModuleMap[counter + 9] = 0x0;
        }

        if ((TempFile = fopen(TempModuleMap, "rb")) == NULL) {
            printf("N\t");
        } else {
            printf("Y\t");
            fclose(TempFile);
        }

        //������������ ����� ����� � ������������ ������ � �������
        TempModuleIO[counter + 3] = 0x2e;
        TempModuleIO[counter + 4] = 0x74;
        TempModuleIO[counter + 5] = 0x78;
        TempModuleIO[counter + 6] = 0x74;
        TempModuleIO[counter + 7] = 0x0;

        if ((TempFile = fopen(TempModuleIO, "rb")) == NULL) {
            printf("N\t");
        } else {
            printf("Y\t");
            fclose(TempFile);
        }

        if (NameFlag == true) {
            //���������� �������� ������, ������� ������� �� ������� ���� �������
            TempFile = fopen(ModuleNamesTable, "rb");
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

