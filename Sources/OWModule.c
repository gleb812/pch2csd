// Function opens an Instr template for Csound

#include <stdio.h>
#include <stdbool.h>

extern FILE *TempFile;

int OWModule(unsigned int number) {
    unsigned int tempnumber;
    unsigned int i, j;
    char TempFileName[50] = "Modules\\"; // fixme
    unsigned int counter = 1;

    tempnumber = number;
    while (true) {
        tempnumber = (tempnumber - tempnumber % 10) / 10;
        if (tempnumber == 0) {
            break;
        }
        counter++;
    }

    for (i = 0; i < counter; i++) {
        tempnumber = number;

        for (j = 0; j < counter - i - 1; j++) {
            tempnumber = tempnumber / 10;
        }
        tempnumber = tempnumber % 10;
        TempFileName[8 + i] = (char) (48 + tempnumber);
    }

    TempFileName[counter + 8] = 0x2e;
    TempFileName[counter + 9] = 0x74;
    TempFileName[counter + 10] = 0x78;
    TempFileName[counter + 11] = 0x74;
    TempFileName[counter + 12] = 0x0;

    OpenWrite(TempFileName);

    return 1;
}
