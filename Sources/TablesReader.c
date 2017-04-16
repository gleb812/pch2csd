
#include <stdio.h>
#include <stdbool.h>

//extern FILE *TempFile;
extern float Tables[128][128];
extern char NamesMapTables[6][256];

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include "Config.h"

int TablesReader(void) {
    unsigned int i;
    unsigned int TableNameCount;
    char NameMapTable[7];

    TableNameCount = 0;

    printf("*** READ ALL TABLES ***\n");

    DIR *dir = opendir(NM_DirTables);
    if (dir) {
        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL) {
            if (strlen(ent->d_name) != 10) { // six chars + .txt
                continue;
            }
            strncpy(NameMapTable, ent->d_name, 6);

            i = 0;

            TableNameCount++;

            for (i = 0; i < 6; i++) {
                NamesMapTables[i][TableNameCount] = NameMapTable[i];
            }

            char tablePath[1024];
            sprintf(tablePath, "%s%s.txt", NM_DirTables, NameMapTable);

            printf(NameMapTable);
            OpenTable(tablePath, TableNameCount);
            printf("\n");

        }
    } else {
        fprintf(stderr, "Error opening directory\n");
    }

    closedir(dir);
/*
    printf("%s",NamesMapTables[1][0]);
    printf("%s",NamesMapTables[1][1]);
    printf("%s",NamesMapTables[1][2]);
    printf("%s",NamesMapTables[1][3]);
    printf("%s",NamesMapTables[1][4]);
    printf("%s",NamesMapTables[1][5],"\n");
*/
    return 0;
};
