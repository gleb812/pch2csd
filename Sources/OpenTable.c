// Function opens a parameter map table
#include <stdio.h>

extern FILE *TempFile;
extern float Tables[128][128];

int OpenTable(char* FileName, unsigned int TableID) {
    float value;
    unsigned int i;

    if ((TempFile = fopen(FileName, "r")) == NULL) {
        printf("  Error - not opened");
        return 0;
    } else {
        printf("  %s", FileName);
        for (i = 0; i < 128; i++) {
            if (fscanf(TempFile, "%f", &value) == EOF) {
                break;
            } else {
                Tables[i][TableID] = value;
            }
        }
    }

    return 1;
}
