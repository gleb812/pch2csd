#include <stdio.h>
#include <stdbool.h>

extern unsigned int IOTable[256][9];

extern FILE *TempFile;

extern unsigned int ModuleCountVA, ModuleCountFX;
extern unsigned int ModuleCounter; // Module Counter
extern unsigned int ModuleListVA[1024];
extern unsigned int ModuleListFX[1024];

extern unsigned int ModuleTypeList[1024]; // Module Type list (modules with the same type are listed only once)
extern unsigned int ModuleTypeCount; // Module type counter

extern unsigned int ModuleIndexListVA[1024]; // VA Module Index list
extern unsigned int ModuleIndexListFX[1024]; // FX Module Index list

extern unsigned int aIOTable[256][6];   //Audio Cables Table (inputs and outputs)
//0-location (VA or FX); 1-Cable number; 2-module from; 3-jack from;
//4-module to;  5-jack to; 6-color; 7-type; 8-a or k type in CSound
extern unsigned int kIOTable[256][6];   //Control Cables Table (inputs and outputs)
//0-location (VA or FX); 1-Cable number; 2-module from; 3-jack from;
//4-module to;  5-jack to; 6-color; 7-type; 8-a or k type in CSound

extern unsigned int CableCounter; // Cable Counter

extern unsigned int azakNumber;
extern unsigned int kzakNumber;

extern unsigned int CCa; // Audio Cable Counter (for csd file)
extern unsigned int CCk; // Control Cable Counter (for csd file)

extern char TempFileName[40];
extern char TempModuleIO[40];

extern bool a2kFlag;
extern bool k2aFlag;

int AddConverters(void) {
    unsigned int i, j, ii;
    unsigned int temp;
    unsigned int tempnumber, counter;
    unsigned int ItempModule, OtempModule;
    unsigned int ItempPort, OtempPort;
    unsigned int Location;
    unsigned int ItempModuleType;

    unsigned char IOtemp;
    unsigned char TempCount;
    unsigned int IO[50];
    unsigned int N;
    unsigned int NIO[50];
    unsigned int IOAK[50]; // Audio = 1; Control =0;
    unsigned int IOCount = 0;
    unsigned int IOTemp[100];

    bool a2kFlag = false;
    bool k2aFlag = false;

    /*
    for(ModuleCounter=0;ModuleCounter<ModuleCountVA;ModuleCounter++)
    {
        printf("Type \t");
        printf("%d\n",ModuleListVA[ModuleCounter]);
    }
    */

    // a - cables
    for (CableCounter = 0; CableCounter < CCa; CableCounter++) {
        Location = aIOTable[CableCounter][0];
        OtempModule = aIOTable[CableCounter][2];
        OtempPort = aIOTable[CableCounter][3];
        ItempModule = aIOTable[CableCounter][4]; // ������ ����������
        ItempPort = aIOTable[CableCounter][5]; // ���� ����������

        printf("Destination Module\t");
        printf("%d\n", ItempModule);

        if (Location == 1) {
            ItempModuleType = ModuleListVA[ItempModule];
        } else {
            ItempModuleType = ModuleListFX[ItempModule];
        }

        printf("Destination Type \t");
        printf("%d\n", ItempModuleType);

        printf("Destination Port\t");
        printf("%d\n", ItempPort);


        // �����������, ��� ��� ������� ���������� �����, � ������ �������� �� ��, ��� �������� ������������� ���� �������, �������� ����������!

        // ������� ����� ������ ������� ��� ������ ����������
        tempnumber = ItempModuleType;
        counter = 1;

        while (true) {
            tempnumber = (tempnumber - tempnumber % 10) / 10;
            if (tempnumber == 0) {
                break;
            }
            counter++;
        }

        for (i = 0; i < counter; i++) {
            tempnumber = ItempModuleType;

            for (j = 0; j < counter - i - 1; j++) {
                tempnumber = tempnumber / 10;
            }
            tempnumber = tempnumber % 10;

            TempModuleIO[3 + i] = (char) (48 + tempnumber);

        }

        //������������ ����� ����� � ������������ ������ � �������
        TempModuleIO[counter + 3] = 0x2e;
        TempModuleIO[counter + 4] = 0x74;
        TempModuleIO[counter + 5] = 0x78;
        TempModuleIO[counter + 6] = 0x74;
        TempModuleIO[counter + 7] = 0x0;

        if ((TempFile = fopen(TempModuleIO, "rb")) == NULL) {
            printf("%s\t", TempModuleIO);
            printf("IO-file not found\n");
        } else {
            IOCount = 0;
            while (true) {
                if (fread(&IOtemp, 1, 1, TempFile) == 0) {
                    break;
                    fclose(TempFile);
                } else {
                    IOTemp[IOCount] = IOtemp;
                    IOCount++;
                }
            }
            //printf("IN'S AND OUT'S\n");
            //printf("READING = ");
            //printf("%d\n",IOCount);

            TempCount = 0;
            for (i = 0; i < IOCount; i++) {
                //printf("%d\n",IOTemp[i]);
                if (i % 4 == 0) {
                    if (IOTemp[i] == 73) // If IN
                    {
                        IO[TempCount] = 0;
                        if (IOTemp[i + 2] == 97) {
                            IOAK[TempCount] = 1; // a
                        }
                        if (IOTemp[i + 2] == 107) {
                            IOAK[TempCount] = 0; // k
                        }
                        TempCount++;
                    }
                    if (IOTemp[i] == 79) // If OUT
                    {
                        IO[TempCount] = 0;
                        if (IOTemp[i + 2] == 97) {
                            IOAK[TempCount] = 1; // a
                        }
                        if (IOTemp[i + 2] == 107) {
                            IOAK[TempCount] = 0; // k
                        }
                        TempCount++;
                    }
                }
            }

            //printf("IO = ");
            //printf("%d\n",TempCount);

            N = 0;
            for (i = 0; i < TempCount; i++) {
                if (IO[i] == 0) {
                    NIO[i] = N;
                    N++;
                }
            }

            N = 0;
            for (i = 0; i < TempCount; i++) {
                if (IO[i] == 1) {
                    NIO[i] = N;
                    N++;
                }
            }

            fclose(TempFile);

            printf("TempCount\t");
            printf("%d\n", TempCount);
            for (i = 0; i < TempCount; i++) {
                printf("i = \t");
                printf("%d\t", i);
                printf("IOAK[i]\t");
                printf("%d\n", IOAK[i]);
            }

            printf("ItempPort \t");
            printf("%d \n", ItempPort);

            printf("AK \t");
            printf("%d \n", IOAK[ItempPort]);

            if (IOAK[ItempPort] == 0) // if this port is k-type
            {

                a2kFlag = true; // this Flag for later add new enhanced opcode for a2k-converter

                if (Location == 1) {
                    ModuleListVA[ModuleCountVA] = 999; // We need add a2k-converter in VA
                    ModuleCountVA++;
                } else {
                    ModuleListFX[ModuleCountFX] = 999; // We need add a2k-converter in FX
                    ModuleCountFX++;
                }

                //We need new k-cable from Converter to Module of Destination

                kIOTable[CCk][0] = Location;
                kIOTable[CCk][1] = CCk;

                if (Location == 1) {
                    temp = 0;
                    for (j = 0; j < ModuleCountVA; j++) {
                        if (temp < ModuleIndexListVA[j]) {
                            temp = ModuleIndexListVA[j];
                        }
                    }
                    kIOTable[CCk][2] = temp + 1;
                } else {
                    temp = 0;
                    for (j = 0; j < ModuleCountFX; j++) {
                        if (temp < ModuleIndexListFX[j]) {
                            temp = ModuleIndexListFX[j];
                        }
                    }
                    kIOTable[CCk][2] = temp + 1;
                }

                kIOTable[CCk][3] = 0;
                kIOTable[CCk][4] = ItempModule;
                kIOTable[CCk][5] = ItempPort;

                CCk++;
                kzakNumber++;

                // And finally we change old a-cable - now it connect the Module with Converter

                if (Location == 1) {
                    aIOTable[CableCounter][4] = temp + 1;
                } else {
                    aIOTable[CableCounter][4] = temp + 1;
                }

                aIOTable[CableCounter][5] = 0;

                if (Location == 1) {
                    ModuleIndexListVA[ModuleCountVA - 1] = temp + 1;
                } else {
                    ModuleIndexListFX[ModuleCountVA - 1] = temp + 1;
                }

                printf("A-cable 2 K-input !!!\t");
                printf("Module \t");
                printf("%d\n", ItempModule);
                printf("Type \t");
                printf("%d\n", ItempModuleType);
                printf("Port \t");
                printf("%d\n", ItempPort);
            }

        }

    }

    // k - cables
    for (CableCounter = 0; CableCounter < CCk; CableCounter++) {
        Location = kIOTable[CableCounter][0];
        OtempModule = kIOTable[CableCounter][2];
        printf("Source Module\t");
        printf("%d\n", OtempModule);

        OtempPort = kIOTable[CableCounter][3];
        printf("Source Port\t");
        printf("%d\n", OtempPort);

        ItempModule = kIOTable[CableCounter][4]; // ������ ����������
        ItempPort = kIOTable[CableCounter][5]; // ���� ����������

        printf("Destination Module\t");
        printf("%d\n", ItempModule);


        if (Location == 1) // VA
        {
            printf("ModuleCountVA\t");
            printf("%d\n", ModuleCountVA);
            for (i = 0; i < ModuleCountVA; i++) {

                printf("i = \t");
                printf("%d\n", i);
                printf("ModuleIndexListVA[i] = \t");
                printf("%d\n", ModuleIndexListVA[i]);


                if (ModuleIndexListVA[i] == ItempModule) {
                    ItempModuleType = ModuleListVA[i];
                    break;
                }

            }
        } else {
            ItempModuleType = ModuleListFX[ItempModule];
        }


        printf("Destination Type \t");
        printf("%d\n", ItempModuleType);

        printf("Destination Port\t");
        printf("%d\n", ItempPort);


        // �����������, ��� ��� ������� ���������� �����, � ������ �������� �� ��, ��� �������� ������������� ���� �������, �������� ����������!

        // ������� ����� ������ ������� ��� ������ ����������
        tempnumber = ItempModuleType;
        counter = 1;

        while (true) {
            tempnumber = (tempnumber - tempnumber % 10) / 10;
            if (tempnumber == 0) {
                break;
            }
            counter++;
        }

        for (i = 0; i < counter; i++) {
            tempnumber = ItempModuleType;

            for (j = 0; j < counter - i - 1; j++) {
                tempnumber = tempnumber / 10;
            }
            tempnumber = tempnumber % 10;

            TempModuleIO[3 + i] = (char) (48 + tempnumber);

        }

        //������������ ����� ����� � ������������ ������ � �������
        TempModuleIO[counter + 3] = 0x2e;
        TempModuleIO[counter + 4] = 0x74;
        TempModuleIO[counter + 5] = 0x78;
        TempModuleIO[counter + 6] = 0x74;
        TempModuleIO[counter + 7] = 0x0;

        if ((TempFile = fopen(TempModuleIO, "rb")) == NULL) {
            printf("%s\t", TempModuleIO);
            printf("IO-file not found\n");
        } else {
            IOCount = 0;
            while (true) {
                if (fread(&IOtemp, 1, 1, TempFile) == 0) {
                    break;
                    fclose(TempFile);
                } else {
                    IOTemp[IOCount] = IOtemp;
                    IOCount++;
                }
            }
            //printf("IN'S AND OUT'S\n");
            //printf("READING = ");
            //printf("%d\n",IOCount);

            TempCount = 0;
            for (i = 0; i < IOCount; i++) {
                //printf("%d\n",IOTemp[i]);
                if (i % 5 == 0) {
                    if (IOTemp[i] == 73) // If IN
                    {
                        IO[TempCount] = 0;
                        if (IOTemp[i + 2] == 97) {
                            IOAK[TempCount] = 1; // a
                        }
                        if (IOTemp[i + 2] == 107) {
                            IOAK[TempCount] = 0; // k
                        }
                        TempCount++;
                    }
                    if (IOTemp[i] == 79) // If OUT
                    {
                        IO[TempCount] = 0;
                        if (IOTemp[i + 2] == 97) {
                            IOAK[TempCount] = 1; // a
                        }
                        if (IOTemp[i + 2] == 107) {
                            IOAK[TempCount] = 0; // k
                        }
                        TempCount++;
                    }
                }
            }

            /*
            printf("IO = ");
            printf("%d\n",TempCount);
            */

            N = 0;
            for (i = 0; i < TempCount; i++) {
                if (IO[i] == 0) {
                    NIO[i] = N;
                    N++;
                }
            }

            N = 0;
            for (i = 0; i < TempCount; i++) {
                if (IO[i] == 1) {
                    NIO[i] = N;
                    N++;
                }
            }

            fclose(TempFile);
            /*
            for(i=0;i<TempCount;i++)
            {
                printf("i = \t");
                printf("%d\t",i);
                printf("IOAK[i]\t");
                printf("%d\n",IOAK[i]);
            }

            printf("ItempPort \t");
            printf("%d \n",ItempPort);

            printf("AK \t");
            printf("%d \n",IOAK[ItempPort]);
            */

            if (IOAK[ItempPort] == 1) // if this port is a-type
            {

                k2aFlag = true; // this Flag for later add new enhanced opcode for a2k-converter

                if (Location == 1) {
                    ModuleListVA[ModuleCountVA] = 1000; // We need add a2k-converter in VA
                    ModuleCountVA++;
                } else {
                    ModuleListFX[ModuleCountFX] = 1000; // We need add a2k-converter in FX
                    ModuleCountFX++;
                }

                //We need new a-cable from Converter to Module of Destination

                aIOTable[CCa][0] = Location;
                aIOTable[CCa][1] = CCa;

                //printf("ModuleCountVA old \t");
                //printf("%d\n",ModuleCountVA-1);

                if (Location == 1) {
                    temp = 0;
                    for (j = 0; j < ModuleCountVA; j++) {
                        if (temp < ModuleIndexListVA[j]) {
                            temp = ModuleIndexListVA[j];
                        }
                    }
                    aIOTable[CCa][2] = temp + 1;
                } else {
                    temp = 0;
                    for (j = 0; j < ModuleCountFX; j++) {
                        if (temp < ModuleIndexListFX[j]) {
                            temp = ModuleIndexListFX[j];
                        }
                    }
                    aIOTable[CCa][2] = temp + 1;
                }

                aIOTable[CCa][3] = 0;
                aIOTable[CCa][4] = ItempModule;
                aIOTable[CCa][5] = ItempPort;

                CCa++;
                azakNumber++;


                // And finally we change old k-cable - now it connect the Module with Converter

                if (Location == 1) {
                    kIOTable[CableCounter][4] = temp + 1;
                } else {
                    kIOTable[CableCounter][4] = temp + 1;
                }

                kIOTable[CableCounter][5] = 0;

                if (Location == 1) {
                    ModuleIndexListVA[ModuleCountVA - 1] = temp + 1;
                } else {
                    ModuleIndexListFX[ModuleCountVA - 1] = temp + 1;
                }

                printf("K-cable 2 A-input !!!\t");
                printf("Module \t");
                printf("%d\n", ItempModule);
                printf("Type \t");
                printf("%d\n", ItempModuleType);
                printf("Port \t");
                printf("%d\n", ItempPort);
            }

        }

    }

    if (k2aFlag == true) {
        ModuleTypeList[ModuleTypeCount] = 1000;
        ModuleTypeCount++;
    }

    if (a2kFlag == true) {
        ModuleTypeList[ModuleTypeCount] = 999;
        ModuleTypeCount++;
    }

    printf("k-cables\n");
    for (i = 0; i < CCk; i++) {

        printf("Location = ");
        printf("%d ", kIOTable[i][0]);
        printf("Number = ");
        printf("%d ", kIOTable[i][1]);
        printf("MF = ");
        printf("%d ", kIOTable[i][2]);
        printf("PF = ");
        printf("%d ", kIOTable[i][3]);
        printf("MT = ");
        printf("%d ", kIOTable[i][4]);
        printf("PT = ");
        printf("%d\n", kIOTable[i][5]);

    }

    printf("a-Cables\n");
    for (i = 0; i < CCa; i++) {
        printf("Location = ");
        printf("%d ", aIOTable[i][0]);
        printf("Number = ");
        printf("%d ", aIOTable[i][1]);
        printf("MF = ");
        printf("%d ", aIOTable[i][2]);
        printf("PF = ");
        printf("%d ", aIOTable[i][3]);
        printf("MT = ");
        printf("%d ", aIOTable[i][4]);
        printf("PT = ");
        printf("%d\n", aIOTable[i][5]);
    }

}
