#include <stdio.h>
#include <string.h>

extern FILE *RecentFile;
extern char RecentFileName[50];
extern char PatchFileName[50];

int menu(void) {
    int returnpointnumber;
    int pointnumber;
    printf("Please make a choice of action!\n");
    printf("1. Convert new patch-file\n");
    printf("2. Convert recent patch-file\n");
    printf("3. Exit\n");
    scanf("%d", &pointnumber);
    switch (pointnumber) {
        case 1: {
            printf("Please write new patch-filename !\n");
            scanf("%s", &PatchFileName);
            returnpointnumber = 0;
            break;
        }
        case 2: {
            if ((RecentFile = fopen(RecentFileName, "r")) == NULL) {
                printf("Error with saving patch-file to recent file!\n");
            } else {
                fgets(PatchFileName, 50, RecentFile);
                fclose(RecentFile);
            }
            returnpointnumber = 0;
            break;
        }
        case 3: {
            returnpointnumber = 1;
            break;
        }
    }

    return returnpointnumber;

}


