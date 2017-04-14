// function that opens a NORD patch file

#include <stdio.h>

extern FILE *ReadFile;
extern FILE *RecentFile;

int OpenPatchFile(const char *PatchFileName) {
    if ((ReadFile = fopen(PatchFileName, "r+b")) == NULL) {
        printf("Error - file not opened!\n");
        return 0;
    }
    return 1;
}
