
#include <stdio.h>
#include <stdbool.h>

//extern FILE *TempFile;
extern float Tables[128][128];
extern char NamesMapTables[6][256];

#include <sys/types.h>
#include <dirent.h>

int TablesReader(void)
{
    unsigned int i,j;
    unsigned int TableNameCount;
    char NameMapTable[6];

    char TableNamesBlank[20]="Tables/xxxxxx.txt"; // Table with

    TableNameCount=0;

    printf("*** READ ALL TABLES ***\n");

    DIR *dir = opendir("Tables");
    if(dir)
    {
        struct dirent *ent;
        while((ent = readdir(dir)) != NULL)
        {
            i=0;
            while(ent->d_name[i]!=NULL)
            {
                NameMapTable[i]=ent->d_name[i];
                i++;
                if(i==6)
                {
                    break;
                }
            }


            //printf("%c\n",NameMapTable[0]);

            if(NameMapTable[0]!=0x2E) // if first symbol not "."
            {
                TableNameCount++;
                //puts(ent->d_name);

                for(i=0;i<6;i++)
                {
                    NamesMapTables[i][TableNameCount]=NameMapTable[i];
                    TableNamesBlank[i+7]=NameMapTable[i];
                }

                OpenTable(TableNamesBlank,TableNameCount);

                printf("%s",TableNamesBlank);
                printf("\n");
            }

/*
            while()
            {
                NamesMapTables[TableNameCount][i]=0x96;
            }
            */
        }
    }
    else
    {
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
