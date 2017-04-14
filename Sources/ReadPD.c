// Function reads the patch file header

#include <stdio.h>
#include <stdbool.h>

extern FILE *ReadFile;

extern unsigned int PDposition;
extern unsigned int PDlength;

// function that reads patch description (PD)
int ReadPD(void) {
    unsigned char temp;
    unsigned int bytecounter = 0;
    unsigned int VoiceCount;
    unsigned int HeightFXVABar;
    bool rcv = false; //Red cable visibility
    bool bcv = false; //Blue cable visibility
    bool ycv = false; //Yellow cable visibility
    bool ocv = false; //Orange cable visibility
    bool gcv = false; //Green cable visibility
    bool pcv = false; //Purple cable visibility
    bool wcv = false; //White cable visibility
    unsigned int MonoPoly;
    unsigned int ActiveVariation;
    unsigned int Category;

    fseek(ReadFile, 0, SEEK_SET);

    while (true) {

        if (fread(&temp, 1, 1, ReadFile) == 0) {
            printf("patch description not found\n");
            return 0;
        } else {
            if (temp == 0x21) {
                printf("*** patch description (PD) ***\n");
                PDposition = bytecounter;
                break;
            }
        }
        bytecounter++;

    }

    printf("PD_position = ");
    printf("%d\n", PDposition);

    fread(&temp, 1, 1, ReadFile);
    PDlength = 0xff * (unsigned int) temp;
    fread(&temp, 1, 1, ReadFile);
    PDlength = PDlength + (unsigned int) temp;

    printf("PD_Length = ");
    printf("%d\n", PDlength);

    fseek(ReadFile, PDposition + 10, SEEK_SET);

    fread(&temp, 1, 1, ReadFile);
    VoiceCount = (unsigned int) ((temp & 0x07) << 2);
    fread(&temp, 1, 1, ReadFile);
    VoiceCount = VoiceCount + (unsigned int) ((temp & 0xc0) >> 6);

    printf("PD_VoiceCount = ");
    printf("%d\n", VoiceCount);

    HeightFXVABar = (unsigned int) ((temp & 0x3f) << 8);
    fread(&temp, 1, 1, ReadFile);
    HeightFXVABar = HeightFXVABar + (unsigned int) temp;

    printf("PD_HeightFXVABar = ");
    printf("%d\n", HeightFXVABar);

    fread(&temp, 1, 1, ReadFile);

    if ((temp & 0x10) > 0) {
        rcv = true;
    }

    if ((temp & 0x08) > 0) {
        bcv = true;
    }

    if ((temp & 0x04) > 0) {
        ycv = true;
    }

    if ((temp & 0x02) > 0) {
        ocv = true;
    }

    if ((temp & 0x01) > 0) {
        gcv = true;
    }

    fread(&temp, 1, 1, ReadFile);

    if ((temp & 0x80) > 0) {
        pcv = true;
    }

    if ((temp & 0x40) > 0) {
        wcv = true;
    }

    if (rcv == true) {
        printf("PD_Red_cable_visibility on\n");
    } else {
        printf("PD_Red_cable_visibility off\n");
    }

    if (bcv == true) {
        printf("PD_Blue_cable_visibility on\n");
    } else {
        printf("PD_Blue_cable_visibility off\n");
    }

    if (ycv == true) {
        printf("PD_Yellow_cable_visibility on\n");
    } else {
        printf("PD_Yellow_cable_visibility off\n");
    }

    if (ocv == true) {
        printf("PD_Orange_cable_visibility on\n");
    } else {
        printf("PD_Orange_cable_visibility off\n");
    }

    if (gcv == true) {
        printf("PD_Green_cable_visibility on\n");
    } else {
        printf("PD_Green_cable_visibility off\n");
    }

    if (pcv == true) {
        printf("PD_Purple_cable_visibility on\n");
    } else {
        printf("PD_Purple_cable_visibility off\n");
    }

    if (wcv == true) {
        printf("PD_White_cable_visibility on\n");
    } else {
        printf("PD_White_cable_visibility off\n");
    }

    MonoPoly = (temp & 0x30) >> 4;

    if (MonoPoly == 0) {
        printf("PD_Mono/Poly = Poly\n");
    }
    if (MonoPoly == 1) {
        printf("PD_Mono/Poly = Mono\n");
    }
    if (MonoPoly == 2) {
        printf("PD_Mono/Poly = Legato\n");
    }

    ActiveVariation = (temp & 0x0f) << 4;
    fread(&temp, 1, 1, ReadFile);
    ActiveVariation = ActiveVariation + ((temp & 0xf0) >> 4);
    ActiveVariation++;

    printf("PD_Active_Variation = ");
    printf("%d\n", ActiveVariation);

    Category = (temp & 0x0f) << 4;
    fread(&temp, 1, 1, ReadFile);
    Category = Category + ((temp & 0xf0) >> 4);

    printf("PD_Category - ");
    switch (Category) {
        case 0:
            printf("No Cat\n");
            break;
        case 1:
            printf("Acoustic\n");
            break;
        case 2:
            printf("Sequencer\n");
            break;
        case 3:
            printf("Bass\n");
            break;
        case 4:
            printf("Classic\n");
            break;
        case 5:
            printf("Drum\n");
            break;
        case 6:
            printf("Fantasy\n");
            break;
        case 7:
            printf("FX\n");
            break;
        case 8:
            printf("Lead\n");
            break;
        case 9:
            printf("Organ\n");
            break;
        case 10:
            printf("Pad\n");
            break;
        case 11:
            printf("Piano\n");
            break;
        case 12:
            printf("Synth\n");
            break;
        case 13:
            printf("Audio In\n");
            break;
        case 14:
            printf("User 1\n");
            break;
        case 15:
            printf("User 2\n");
            break;
    }
    return 1;
}
