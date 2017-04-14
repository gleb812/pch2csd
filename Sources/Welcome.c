#include <stdio.h>

void Help(void) {
    printf("Clavia Nord Modular G2 patch to Csound converter, ver. 0.1\n");
    printf("\n");
    printf("Usage:\n");
    printf("  pch2csd nm2_patch_file.pch2 [-d data_dir]\n");
    printf("\n");
    printf("Options:\n");
    printf("  -d Path to a data-directory (maps, UDOs, etc.). Default is './Data/'\n");
}
