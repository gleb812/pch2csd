// Function reads a list of cables with their colors and connections to modules and jacks
// RED and ORANGE are a-type
// BLUE and YELLOW are k-type
// OTHERS are unknown type

// All data are written in IOTable
// Cable list (inputs and outputs)
//0-location (VA or FX); 1-Cable number; 2-module from; 3-jack from;
//4-module to;  5-jack to; 6-color; 7-type; 8-a or k type in CSound

#include <stdio.h>
#include <stdbool.h>

extern FILE *ReadFile;

extern unsigned int CLposition;
extern unsigned int CLlength;

extern unsigned int CableCounter;
extern unsigned int SoundCableCount;
extern unsigned int ControlCableCount;

extern unsigned int OtherCableCount;
extern unsigned int IOTable[256][9];

int ReadCL(unsigned int position) {
    unsigned char temp;
    unsigned int bytecounter = 0;
    bool Data[10240];
    const unsigned int CableLength = 32;
    bool Cable[CableLength];
    unsigned int i, j;
    bool location;
    unsigned int CableCount;
    unsigned int Color;
    unsigned int AK; //Audio or Control (a or k)
    unsigned int ModuleFrom;
    unsigned int JackFrom;
    bool CableType;
    unsigned int ModuleTo;
    unsigned int JackTo;

    fseek(ReadFile, position, SEEK_SET);

    while (true) {

        if (fread(&temp, 1, 1, ReadFile) == 0) {
            printf("cable list not found\n");
            return 0;
        } else {
            if (temp == 0x52) {
                printf("*** cable list (CL) ***\n");
                CLposition = position + bytecounter;
                break;
            }
        }
        bytecounter++;

    }

    //printf("CL_position = ");
    //printf("%d\n",CLposition);

    fread(&temp, 1, 1, ReadFile);
    CLlength = 0xff * (unsigned int) temp;
    fread(&temp, 1, 1, ReadFile);
    CLlength = CLlength + (unsigned int) temp;

    //printf("CL_Length = ");
    //printf("%d\n",CLlength);

    for (i = 0; i < CLlength * 8; i++) {
        if ((i % 8) == 0) {
            fread(&temp, 1, 1, ReadFile);

            for (j = 0; j < 8; j++) {
                if (((temp >> (8 - (j + 1))) & 0x01) > 0) {
                    Data[i + j] = true;
                } else {
                    Data[i + j] = false;
                }
            }
        }
    }

    location = Data[1];

    printf("CL_Location = ");
    if (location == 0) {
        printf("FX Area\n");
    } else {
        printf("Voice Area\n");
    }

    CableCount =
            0x80 * Data[16] + 0x40 * Data[17] + 0x20 * Data[18] + 0x10 * Data[19] + 0x08 * Data[20] + 0x04 * Data[21] +
            0x02 * Data[22] + Data[23];

    printf("CL_Cable_Count = ");
    printf("%d\n", CableCount);

    for (i = 0; i < CableCount; i++) {
        for (j = 0; j < CableLength; j++) {
            Cable[j] = Data[24 + i * CableLength + j];
        }

        printf("CL_Cable_#");
        printf("%d\n", i);

        Color = 0x04 * Cable[0] + 0x02 * Cable[1] + Cable[2];

        printf("CL_Color = ");
        printf("%d", Color);
        printf(" - ");
        switch (Color) {
            case 0:
                printf("Red");
                SoundCableCount++;
                AK = 1;
                break;
            case 1:
                printf("Blue");
                ControlCableCount++;
                AK = 0;
                break;
            case 2:
                printf("Yellow");
                ControlCableCount++;
                AK = 0;
                break;
            case 3:
                printf("Orange");
                SoundCableCount++;
                AK = 1;
                break;
            case 4:
                printf("Green");
                OtherCableCount++;
                AK = 2;
                break;
            case 5:
                printf("Purple");
                OtherCableCount++;
                AK = 2;
                break;
            case 6:
                printf("White");
                OtherCableCount++;
                AK = 2;
                break;
            default:
                printf("unknown");
                OtherCableCount++;
                AK = 2;
        }

        printf("\n");

        ModuleFrom = 0x80 * Cable[3] + 0x40 * Cable[4] + 0x20 * Cable[5] + 0x10 * Cable[6] + 0x08 * Cable[7] +
                     0x04 * Cable[8] + 0x02 * Cable[9] + Cable[10];

        printf("CL_Module_From - ");
        printf("%d\n", ModuleFrom);

        JackFrom = 0x20 * Cable[11] + 0x10 * Cable[12] + 0x08 * Cable[13] + 0x04 * Cable[14] + 0x02 * Cable[15] +
                   Cable[16];

        printf("CL_Jack_From - ");
        printf("%d\n", JackFrom);

        printf("Cable[16] - ");
        printf("%d\n", Cable[16]);

        CableType = Cable[17];

        printf("CL_Type = ");
        printf("%d", CableType);
        printf(" - ");

        switch (CableType) {
            case 0:
                printf("Input to Input\n");
                break;
            case 1:
                printf("Output to Input\n");
                break;
        }


        ModuleTo = 0x80 * Cable[18] + 0x40 * Cable[19] + 0x20 * Cable[20] + 0x10 * Cable[21] + 0x08 * Cable[22] +
                   0x04 * Cable[23] + 0x02 * Cable[24] + Cable[25];

        printf("CL_Module_To - ");
        printf("%d\n", ModuleTo);

        JackTo = 0x20 * Cable[26] + 0x10 * Cable[27] + 0x08 * Cable[28] + 0x04 * Cable[29] + 0x02 * Cable[30] +
                 Cable[31];

        printf("CL_Jack_To - ");
        printf("%d\n", JackTo);

        IOTable[CableCounter][0] = (unsigned int) location;
        IOTable[CableCounter][1] = i;
        IOTable[CableCounter][2] = ModuleFrom;
        IOTable[CableCounter][3] = JackFrom;
        IOTable[CableCounter][4] = ModuleTo;
        IOTable[CableCounter][5] = JackTo;
        IOTable[CableCounter][6] = AK;
        IOTable[CableCounter][7] = CableType;
        IOTable[CableCounter][8] = AK;

        CableCounter++;
    }

    return 1;

}
