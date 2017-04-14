// Opens a new file

#include <stdio.h>
#include <stdbool.h>

extern FILE *TempFile;
extern FILE *NewFile;

int OpenWrite(const char* TempFileName) {
    char temp;
    if ((TempFile = fopen(TempFileName, "r")) == NULL) {
        /*
        printf("Error - ");
        printf(TempFileName);
        printf(" not opened!\n");
        */
        return 0;
    } else {
        while (true) {
            if (fread(&temp, 1, 1, TempFile) != 0) {
                fprintf(NewFile, "%c", temp);
            } else {
                fprintf(NewFile, "\n");
                break;
            }
        }
    }

    return 1;
}
