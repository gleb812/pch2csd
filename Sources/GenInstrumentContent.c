// Function that adds Instr to orc part according to Nord patch
#include <stdio.h>
#include <stdbool.h>

extern FILE *NewFile;
extern FILE *TempFile;

extern unsigned int CCa,CCk;

extern char TempFileName[40];
extern char TempModuleMap[40];
extern char TempModuleIO[40];

extern unsigned int aIOTable[256][6]; // a-cable list
                                //0-location (VA or FX); 1-cable ID; 2-module from; 3-port from;
                                //4-module to;  5-port to;
extern unsigned int kIOTable[256][6];// k-cable list
                                //0-location (VA or FX); 1-cable ID; 2-module from; 3-port from;
                                //4-module to;  5-port to;

extern char NamesMapTables[6][256]; // Six Symbol Name Format - example CLA000

extern unsigned int ParameterCountersVA[128]; // Mapping tables to map the parameters into Csound values (Tables[...])
                                                //for VA field
extern unsigned int ParameterCountersFX[128]; // same for FX field

extern unsigned int MapTablesVA[128][24]; // Containd a column numbers of mapping Tables,so we open the map file for the given module,
                                            // where parameter IDs are written, i.e. 1 means frequency etc.
extern unsigned int MapTablesFX[128][24]; // the same

extern unsigned int ParametersVA[128][64]; // VA parameters
extern unsigned int ParametersFX[128][64]; // FX parameters

extern unsigned int ModuleCounter; // Module Counter

extern unsigned int ModuleIndexListVA[1024]; // VA modules index list
extern unsigned int ModuleIndexListFX[1024]; // same for FX

extern float Tables[128][128]; // Parameter mapping table

extern bool VAFXFlag; // VA or FX flag


int GenInstrumentContent(unsigned int number)
{
    const unsigned int L=40;
    unsigned int tempnumber;
    unsigned int i,j,k;
    char NAME[L];
    unsigned int NAMELength=0;
    char TEMP[L];
    unsigned int counter=1;
    char temp;
    unsigned int valueINT;
    unsigned int mapid;
    float value[64];
    unsigned int IOTemp[100];
    char ParameterType[100];
    unsigned int StringCounter;
    unsigned int IOCount=0;
    unsigned int TempCount=0;
    unsigned char IOtemp;
    unsigned char Maptemp;
    unsigned char Maptemp6[6];
    unsigned int SelectorID;
    unsigned int SelectorIDadd;
    unsigned int TablesPointer;
    unsigned char Nametemp6[6];
    unsigned int IO[50]; // Input = 0; Output = 0;
    unsigned int N;
    unsigned int NIO[50]; // Input = 0; Output = 0;
    unsigned int IOAK[50]; // Audio = 1; Control =0;
    bool FFlag;
    bool PPflag; // To understand whether to put a coma between parameters

    tempnumber=number;

    //printf("Number = %d\n",number);
    //printf("ModuleCounter = %d\n",ModuleCounter);

    while(true)
    {
        tempnumber=(tempnumber-tempnumber%10)/10;
        if(tempnumber==0)
        {
            break;
        }
        counter++;
    }

    for(i=0;i<counter;i++)
    {
        tempnumber=number;

        for(j=0;j<counter-i-1;j++)
        {
            tempnumber=tempnumber/10;
        }
        tempnumber=tempnumber%10;
        TempFileName[8+i]=(char)(48+tempnumber);
        TempModuleMap[5+i]=(char)(48+tempnumber);
        TempModuleIO[3+i]=(char)(48+tempnumber);

    }

    //Module Name
    TempFileName[counter+8]=0x2e;
    TempFileName[counter+9]=0x74;
    TempFileName[counter+10]=0x78;
    TempFileName[counter+11]=0x74;
    TempFileName[counter+12]=0x0;

    //Mapping file name
    TempModuleMap[counter+5]=0x2e;
    TempModuleMap[counter+6]=0x74;
    TempModuleMap[counter+7]=0x78;
    TempModuleMap[counter+8]=0x74;
    TempModuleMap[counter+9]=0x0;

    //формирование имени файла с определением входов и выходов
    TempModuleIO[counter+3]=0x2e;
    TempModuleIO[counter+4]=0x74;
    TempModuleIO[counter+5]=0x78;
    TempModuleIO[counter+6]=0x74;
    TempModuleIO[counter+7]=0x0;

    PPflag=false;


    //We need only name of a module
    if((TempFile = fopen(TempFileName,"r")) == NULL)
	{
		//printf("Error - ");
		//printf(TempFileName);
		//printf(" not opened!\n");
		return 0;
	}
	else
	{
	    fseek(TempFile, 0, SEEK_SET);
	    for(i=0;i<L;i++)
        {
            if(fread(&temp,1,1,TempFile) != 0)
            {
                TEMP[i]=temp;
                if(temp==0x2c)
                {
                    for(j=i-1;j>0;j--)
                    {
                        if((TEMP[j]==0x09)||(TEMP[j]==0x20))
                        {
                            for(k=j;k<i;k++)
                            {
                                NAME[k-j]=TEMP[k];
                                NAMELength++;
                            }
                            break;
                        }
                    }
                    break;
                }
            }
            else
            {
                break;
            }
        }
        fclose(TempFile);
	}
	fprintf(NewFile,"\t");
	//printf("%d\n",NAMELength);
	// Write a name of the module to csd
	for(i=0;i<NAMELength;i++)
    {
        fprintf(NewFile,"%c",NAME[i]);
    }
    fprintf(NewFile," ");


    // Now we open a map file and read the parameters according to current index
    // We find the parameter values for the current module index, look for it into a mapping table, since we know the number of column
    // Write parameters to csd file


    if(VAFXFlag)
    {

        if((TempFile = fopen(TempModuleMap,"rb")) == NULL)
        {
            return 0;
        }
        else
        {
            //printf("Number of real parameters =");

            fseek(TempFile, 0, SEEK_SET);
            StringCounter=0;
            while(true)
            {
                if(fread(&Maptemp,1,1,TempFile)==0)
                {
                    break;
                }
                if(Maptemp==13)
                {
                    StringCounter++;
                }

            }
            StringCounter++;
            //printf("%d\n",StringCounter);

            fseek(TempFile, 0, SEEK_SET);

            //printf("Direct Parameters\n");

            for(i=0;i<StringCounter;i++)
            {
                fread(&Maptemp,1,1,TempFile);

                ParameterType[i]=Maptemp;

                if(Maptemp==0)
                {
                    break;
                }

                if((Maptemp==100)||(Maptemp==104)) // d or h
                {

                    fread(&Maptemp,1,1,TempFile); // space or tab

                    fread(&Maptemp6,1,6,TempFile);
                    for(j=0;j<256;j++)
                    {
                        for(k=0;k<6;k++)
                        {
                            Nametemp6[k]=NamesMapTables[k][j];
                        }

                        if(memcmp(Maptemp6,Nametemp6,6)==0)
                        {
                            mapid=j;

                            MapTablesVA[ModuleCounter][i]=mapid;
                            valueINT=ParametersVA[ModuleCounter][i];
                            value[i]=Tables[valueINT][mapid];

                            break;

                        }
                    }
                }

                while(true)
                {
                    if(fread(&Maptemp,1,1,TempFile)==0)
                    {
                        break;
                    }
                    if(Maptemp==13)
                    {
                        fread(&Maptemp,1,1,TempFile);
                        break;
                    }

                }

            }

            //printf("Selected Parameters\n");

            fseek(TempFile, 0, SEEK_SET);
            for(i=0;i<StringCounter;i++)
            {
                fread(&Maptemp,1,1,TempFile);

                if(Maptemp==115)
                {
                    fread(&Maptemp,1,1,TempFile); // space or tab

                    SelectorID=0;
                    while(true)
                    {
                        fread(&Maptemp,1,1,TempFile);
                        if((Maptemp==13)||(Maptemp==9)||(Maptemp==32)||(Maptemp==0))
                        {
                            break;
                        }
                        SelectorID=SelectorID*10+(Maptemp-48);
                    }

                    TablesPointer=(unsigned int)(value[SelectorID-1]);

                    for(j=0;j<TablesPointer+1;j++)
                    {
                        fread(&Maptemp6,1,6,TempFile);
                        fread(&Maptemp,1,1,TempFile);
                    }

                    for(j=0;j<256;j++)
                    {
                        for(k=0;k<6;k++)
                        {
                            Nametemp6[k]=NamesMapTables[k][j];
                        }

                        if(memcmp(Maptemp6,Nametemp6,6)==0)
                        {
                            mapid=j;

                            MapTablesVA[ModuleCounter][i]=mapid;
                            valueINT=ParametersVA[ModuleCounter][i];
                            value[i]=Tables[valueINT][mapid];

                            break;

                        }
                    }
                }

                while(true)
                {
                    if(fread(&Maptemp,1,1,TempFile)==0)
                    {
                        break;
                    }
                    if(Maptemp==13)
                    {
                        fread(&Maptemp,1,1,TempFile);
                        break;
                    }

                }

            }

            // And let set the w-map parameters (Section of Delays)
            //

            fseek(TempFile, 0, SEEK_SET);
            for(i=0;i<StringCounter;i++)
            {
                fread(&Maptemp,1,1,TempFile);

                if(Maptemp==119) // if find w-type parameter
                {
                    fread(&Maptemp,1,1,TempFile); // space or tab

                    SelectorID=0; // first selector. His value most be only "0" or "1"
                    while(true)
                    {
                        fread(&Maptemp,1,1,TempFile);
                        if((Maptemp==13)||(Maptemp==9)||(Maptemp==32)||(Maptemp==0))
                        {
                            break;
                        }
                        SelectorID=SelectorID*10+(Maptemp-48);
                    }

                    SelectorIDadd=0; // second selector
                    while(true)
                    {
                        fread(&Maptemp,1,1,TempFile);
                        if((Maptemp==13)||(Maptemp==9)||(Maptemp==32)||(Maptemp==0))
                        {
                            break;
                        }
                        SelectorIDadd=SelectorIDadd*10+(Maptemp-48);
                    }

                    TablesPointer=(unsigned int)(value[SelectorID-1])*(unsigned int)(value[SelectorIDadd-1]);

                    for(j=0;j<TablesPointer+1;j++)
                    {
                        fread(&Maptemp6,1,6,TempFile);
                        fread(&Maptemp,1,1,TempFile);
                    }

                    for(j=0;j<256;j++)
                    {
                        for(k=0;k<6;k++)
                        {
                            Nametemp6[k]=NamesMapTables[k][j];
                        }

                        if(memcmp(Maptemp6,Nametemp6,6)==0)
                        {
                            mapid=j;

                            MapTablesVA[ModuleCounter][i]=mapid;
                            valueINT=ParametersVA[ModuleCounter][i];
                            value[i]=Tables[valueINT][mapid];

                            break;

                        }
                    }
                }

                while(true)
                {
                    if(fread(&Maptemp,1,1,TempFile)==0)
                    {
                        break;
                    }
                    if(Maptemp==13)
                    {
                        fread(&Maptemp,1,1,TempFile);
                        break;
                    }

                }

            }

        }

        for(i=0;i<StringCounter;i++)
        {
            mapid=MapTablesVA[ModuleCounter][i];

            printf("ParameterType: ");
            printf("%c\n",ParameterType[i]);

            for(k=0;k<6;k++)
            {
                Nametemp6[k]=NamesMapTables[k][mapid];
            }

            printf("TableName: ");
            for(k=0;k<6;k++)
            {
                printf("%c",Nametemp6[k]);
            }

            printf("; ");
            printf("TableID = ");
            printf("%d\n",mapid);

            printf("%d",ParametersVA[ModuleCounter][i]);
            printf(" -> ");
            printf("%1.3f\n",value[i]);


            if(ParameterType[i]!=104) // h - we hide parameter in new.csd-file
            {
                fprintf(NewFile,"%1.3f",value[i]);
                PPflag=true;
                if((i!=ParameterCountersVA[ModuleCounter]-1)&&(ParameterType[i+1]!=104))
                {
                    fprintf(NewFile,", ");
                }
            }

        }
        //fprintf(NewFile,"\n");
    }
    else
    {
        if((TempFile = fopen(TempModuleMap,"rb")) == NULL)
        {
            return 0;
        }
        else
        {
            //printf("Number of real parameters =");

            fseek(TempFile, 0, SEEK_SET);
            StringCounter=0;
            while(true)
            {
                if(fread(&Maptemp,1,1,TempFile)==0)
                {
                    break;
                }
                if(Maptemp==13)
                {
                    StringCounter++;
                }

            }
            StringCounter++;
            //printf("%d\n",StringCounter);

            fseek(TempFile, 0, SEEK_SET);

            //printf("Direct Parameters\n");

            for(i=0;i<StringCounter;i++)
            {
                fread(&Maptemp,1,1,TempFile);

                ParameterType[i]=Maptemp;

                if(Maptemp==0)
                {
                    break;
                }

                if((Maptemp==100)||(Maptemp==104)) // d or h
                {

                    fread(&Maptemp,1,1,TempFile); // space or tab

                    fread(&Maptemp6,1,6,TempFile);
                    for(j=0;j<256;j++)
                    {
                        for(k=0;k<6;k++)
                        {
                            Nametemp6[k]=NamesMapTables[k][j];
                        }

                        if(memcmp(Maptemp6,Nametemp6,6)==0)
                        {
                            mapid=j;

                            MapTablesFX[ModuleCounter][i]=mapid;
                            valueINT=ParametersFX[ModuleCounter][i];
                            value[i]=Tables[valueINT][mapid];

                            break;

                        }
                    }
                }

                while(true)
                {
                    if(fread(&Maptemp,1,1,TempFile)==0)
                    {
                        break;
                    }
                    if(Maptemp==13)
                    {
                        fread(&Maptemp,1,1,TempFile);
                        break;
                    }

                }

            }

            //printf("Selected Parameters\n");

            fseek(TempFile, 0, SEEK_SET);
            for(i=0;i<StringCounter;i++)
            {
                fread(&Maptemp,1,1,TempFile);

                if(Maptemp==115)
                {
                    fread(&Maptemp,1,1,TempFile); // space or tab

                    SelectorID=0;
                    while(true)
                    {
                        fread(&Maptemp,1,1,TempFile);
                        if((Maptemp==13)||(Maptemp==9)||(Maptemp==32)||(Maptemp==0))
                        {
                            break;
                        }
                        SelectorID=SelectorID*10+(Maptemp-48);
                    }

                    TablesPointer=(unsigned int)(value[SelectorID-1]);

                    for(j=0;j<TablesPointer+1;j++)
                    {
                        fread(&Maptemp6,1,6,TempFile);
                        fread(&Maptemp,1,1,TempFile);
                    }

                    for(j=0;j<256;j++)
                    {
                        for(k=0;k<6;k++)
                        {
                            Nametemp6[k]=NamesMapTables[k][j];
                        }

                        if(memcmp(Maptemp6,Nametemp6,6)==0)
                        {
                            mapid=j;

                            MapTablesFX[ModuleCounter][i]=mapid;
                            valueINT=ParametersFX[ModuleCounter][i];
                            value[i]=Tables[valueINT][mapid];

                            break;

                        }
                    }
                }

                while(true)
                {
                    if(fread(&Maptemp,1,1,TempFile)==0)
                    {
                        break;
                    }
                    if(Maptemp==13)
                    {
                        fread(&Maptemp,1,1,TempFile);
                        break;
                    }

                }

            }

            // And let set the w-map parameters (Section of Delays)
            //

            fseek(TempFile, 0, SEEK_SET);
            for(i=0;i<StringCounter;i++)
            {
                fread(&Maptemp,1,1,TempFile);

                if(Maptemp==119) // if find w-type parameter
                {
                    fread(&Maptemp,1,1,TempFile); // space or tab

                    SelectorID=0; // first selector. His value most be only "0" or "1"
                    while(true)
                    {
                        fread(&Maptemp,1,1,TempFile);
                        if((Maptemp==13)||(Maptemp==9)||(Maptemp==32)||(Maptemp==0))
                        {
                            break;
                        }
                        SelectorID=SelectorID*10+(Maptemp-48);
                    }

                    SelectorIDadd=0; // second selector
                    while(true)
                    {
                        fread(&Maptemp,1,1,TempFile);
                        if((Maptemp==13)||(Maptemp==9)||(Maptemp==32)||(Maptemp==0))
                        {
                            break;
                        }
                        SelectorIDadd=SelectorIDadd*10+(Maptemp-48);
                    }

                    TablesPointer=(unsigned int)(value[SelectorID-1])*(unsigned int)(value[SelectorIDadd-1]);

                    for(j=0;j<TablesPointer+1;j++)
                    {
                        fread(&Maptemp6,1,6,TempFile);
                        fread(&Maptemp,1,1,TempFile);
                    }

                    for(j=0;j<256;j++)
                    {
                        for(k=0;k<6;k++)
                        {
                            Nametemp6[k]=NamesMapTables[k][j];
                        }

                        if(memcmp(Maptemp6,Nametemp6,6)==0)
                        {
                            mapid=j;

                            MapTablesFX[ModuleCounter][i]=mapid;
                            valueINT=ParametersFX[ModuleCounter][i];
                            value[i]=Tables[valueINT][mapid];

                            break;

                        }
                    }
                }

                while(true)
                {
                    if(fread(&Maptemp,1,1,TempFile)==0)
                    {
                        break;
                    }
                    if(Maptemp==13)
                    {
                        fread(&Maptemp,1,1,TempFile);
                        break;
                    }

                }

            }

        }

        for(i=0;i<StringCounter;i++)
        {
            mapid=MapTablesFX[ModuleCounter][i];

            printf("ParameterType: ");
            printf("%c\n",ParameterType[i]);

            for(k=0;k<6;k++)
            {
                Nametemp6[k]=NamesMapTables[k][mapid];
            }

            printf("TableName: ");
            for(k=0;k<6;k++)
            {
                printf("%c",Nametemp6[k]);
            }

            printf("; ");
            printf("TableID = ");
            printf("%d\n",mapid);

            printf("%d",ParametersFX[ModuleCounter][i]);
            printf(" -> ");
            printf("%1.3f\n",value[i]);


            if(ParameterType[i]!=104) // h - we hide parameter in new.csd-file
            {
                fprintf(NewFile,"%1.3f",value[i]);
                PPflag=true;
                if((i!=ParameterCountersFX[ModuleCounter]-1)&&(ParameterType[i+1]!=104))
                {
                    fprintf(NewFile,", ");
                }
            }

        }
        //fprintf(NewFile,"\n");
    }


    // Here goes the weirdest thing - patching
    // The same way we open IO file with inputs, outputs and their types for given module

    // first column input(0)/output(1)
    // second column k(0)/a(1)

    if((TempFile = fopen(TempModuleIO,"r")) == NULL)
    {
        //printf("Error - ");
        //printf(TempModuleIO);
        //printf(" not opened!\n");
        return 0;
    }
    else
    {
        IOCount=0;
        while(true)
        {
            if(fread(&IOtemp,1,1,TempFile) == 0)
            {
                break;
                fclose(TempFile);
            }
            else
            {
                IOTemp[IOCount]=IOtemp;
                IOCount++;
            }
        }
        //printf("IN'S AND OUT'S\n");
        //printf("READING = ");
        //printf("%d\n",IOCount);

        TempCount=0;
        for(i=0;i<IOCount;i++)
        {
            //printf("%d\n",IOTemp[i]);
            if(i%4==0)
            {
                if(IOTemp[i]==73) // If IN
                {
                    IO[TempCount]=0;
                    if(IOTemp[i+2]==97)
                    {
                        IOAK[TempCount]=1; // a
                    }
                    if(IOTemp[i+2]==107)
                    {
                        IOAK[TempCount]=0; // k
                    }
                    TempCount++;
                }
                if(IOTemp[i]==79) // If OUT
                {
                    IO[TempCount]=0;
                    if(IOTemp[i+2]==97)
                    {
                        IOAK[TempCount]=1; // a
                    }
                    if(IOTemp[i+2]==107)
                    {
                        IOAK[TempCount]=0; // k
                    }
                    TempCount++;
                }
            }
        }

        //printf("IO = ");
        //printf("%d\n",TempCount);

        N=0;
        for(i=0;i<TempCount;i++)
        {
            if(IO[i]==0)
            {
                NIO[i]=N;
                N++;
            }
        }

        N=0;
        for(i=0;i<TempCount;i++)
        {
            if(IO[i]==1)
            {
                NIO[i]=N;
                N++;
            }
        }
    }

    if(TempCount>0)
    {
        if(PPflag)
        {
            fprintf(NewFile,", ");
        }
    }
// Once all inputs and outputs are read and NIO table are created (it carries the numbers of inputs and outputs)
// we have to check each element of table
// if we find the value equal to module and port, we create a patch in zak space
// if the corresponding cable doesn't exsist & it is input, we patch it to zero bus (works like a ground)
// if it is output, we patch it to bus 1 (a trash bus).


    if(VAFXFlag) //VA Area
    {
        for(i=0;i<TempCount;i++)
        {
            FFlag=false;
            if(IOAK[i]==1) // output type is audio
            {
                if(IO[i]==0) // if input
                {
                    //printf("IN\n");
                    for(j=0;j<CCa;j++)
                    {
                        //printf("Moduls %d %d\n",ModuleIndexListVA[ModuleCounter],aIOTable[j][4]);
                        //printf("Ports %d %d\n",NIO[i],aIOTable[j][5]);
                        if(aIOTable[j][4]==ModuleIndexListVA[ModuleCounter]) //module to should be equal to module number
                        {
                            if(aIOTable[j][5]==NIO[i]) //port should be equal to port number from IO file
                            {
                                if(aIOTable[j][0]==1) // Should be VA part
                                {
                                    fprintf(NewFile,"%d",(aIOTable[j][1]+2)); //Write a number of cable
                                    if(i!=TempCount-1)
                                    {
                                        fprintf(NewFile,", ");
                                    }
                                    FFlag=true;
                                    break;
                                }
                            }
                        }
                    }
                }
                else // if output
                {
                    //printf("OUT\n");
                    for(j=0;j<CCa;j++)
                    {
                        //printf("Moduls %d %d\n",ModuleIndexListVA[ModuleCounter],aIOTable[j][2]);
                        //printf("Ports %d %d\n",NIO[i],aIOTable[j][3]);
                        if(aIOTable[j][2]==ModuleIndexListVA[ModuleCounter]) //module to should be equal to module number
                        {
                            if(aIOTable[j][3]==NIO[i]) //port should be equal to port number from IO file
                            {
                                if(aIOTable[j][0]==1) // Should be VA part
                                {
                                    fprintf(NewFile,"%d",(aIOTable[j][1]+2)); //Write a number of cable
                                    if(i!=TempCount-1)
                                    {
                                        fprintf(NewFile,", ");
                                    }
                                    FFlag=true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(FFlag==false)
                {
                    fprintf(NewFile,"%d",IO[i]); //if it is input, we connect it to 0 bus. If it is input, we use a trash bus 1
                    if(i!=TempCount-1)
                    {
                        fprintf(NewFile,", ");
                    }
                }
            }
            else // k type
            {
                if(IO[i]==0) // if input
                {
                    //printf("IN\n");
                    for(j=0;j<CCk;j++)
                    {
                        //printf("Moduls %d %d\n",ModuleIndexListVA[ModuleCounter],kIOTable[j][4]);
                        //printf("Ports %d %d\n",NIO[i],kIOTable[j][5]);
                        if(kIOTable[j][4]==ModuleIndexListVA[ModuleCounter]) //module to should be equal to module number
                        {
                            if(kIOTable[j][5]==NIO[i]) //port should be equal to port number from IO file
                            {
                                if(aIOTable[j][0]==1) // Should be VA part
                                {
                                    fprintf(NewFile,"%d",(kIOTable[j][1]+2)); //Write a number of cable
                                    if(i!=TempCount-1)
                                    {
                                        fprintf(NewFile,", ");
                                    }
                                    FFlag=true;
                                    break;
                                }
                            }
                        }
                    }
                }
                else // if output
                {
                    //printf("OUT\n");
                    for(j=0;j<CCk;j++)
                    {
                        //printf("Moduls %d %d\n",ModuleIndexListVA[ModuleCounter],kIOTable[j][2]);
                        //printf("Ports %d %d\n",NIO[i],kIOTable[j][3]);
                        if(kIOTable[j][2]==ModuleIndexListVA[ModuleCounter]) //module to should be equal to module number
                        {
                            if(kIOTable[j][3]==NIO[i]) //port should be equal to port number from IO file
                            {
                                if(aIOTable[j][0]==1) // Should be VA part
                                {
                                    fprintf(NewFile,"%d",(kIOTable[j][1]+2)); //Write a number of cable
                                    if(i!=TempCount-1)
                                    {
                                        fprintf(NewFile,", ");
                                    }
                                    FFlag=true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(FFlag==false)
                {
                    fprintf(NewFile,"%d",IO[i]); //if it is input, we connect it to 0 bus. If it is input, we use a trash bus 1
                    if(i!=TempCount-1)
                    {
                        fprintf(NewFile,", ");
                    }
                }
            }
        }
    }
    else // FX Area
    {
        for(i=0;i<TempCount;i++)
        {
            FFlag=false;
            if(IOAK[i]==1) // audio type
            {
                if(IO[i]==0) // if input
                {
                    //printf("IN\n");
                    for(j=0;j<CCa;j++)
                    {
                        //printf("Moduls %d %d\n",ModuleIndexListFX[ModuleCounter],aIOTable[j][4]);
                        //printf("Ports %d %d\n",NIO[i],aIOTable[j][5]);
                        if(aIOTable[j][4]==ModuleIndexListFX[ModuleCounter]) //module to should be equal to module number
                        {
                            if(aIOTable[j][5]==NIO[i]) //port should be equal to port number from IO file
                            {
                                if(aIOTable[j][0]==0) // Should be FX part
                                {
                                    fprintf(NewFile,"%d",(aIOTable[j][1]+2)); //Write a number of cable
                                    if(i!=TempCount-1)
                                    {
                                        fprintf(NewFile,", ");
                                    }
                                    FFlag=true;
                                    break;
                                }
                            }
                        }
                    }
                }
                else // if output
                {
                    //printf("OUT\n");
                    for(j=0;j<CCa;j++)
                    {
                        //printf("Moduls %d %d\n",ModuleIndexListFX[ModuleCounter],aIOTable[j][2]);
                        //printf("Ports %d %d\n",NIO[i],aIOTable[j][3]);
                        if(aIOTable[j][2]==ModuleIndexListFX[ModuleCounter]) //module to should be equal to module number
                        {
                            if(aIOTable[j][3]==NIO[i]) //port should be equal to port number from IO file
                            {
                                if(aIOTable[j][0]==0) // Should be FX part
                                {
                                    fprintf(NewFile,"%d",(aIOTable[j][1]+2)); //Write a number of cable
                                    if(i!=TempCount-1)
                                    {
                                        fprintf(NewFile,", ");
                                    }
                                    FFlag=true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(FFlag==false)
                {
                    //fprintf(NewFile,"%d",IO[i]); //if it is input, we connect it to 0 bus. If it is input, we use a trash bus 1
                    if(i!=TempCount-1)
                    {
                        fprintf(NewFile,", ");
                    }
                }
            }
            else // k signal
            {
                if(IO[i]==0) // if input
                {
                    //printf("IN\n");
                    for(j=0;j<CCk;j++)
                    {
                        //printf("Moduls %d %d\n",ModuleIndexListFX[ModuleCounter],kIOTable[j][4]);
                        //printf("Ports %d %d\n",NIO[i],kIOTable[j][5]);
                        if(kIOTable[j][4]==ModuleIndexListFX[ModuleCounter]) //module to should be equal to module number
                        {
                            if(kIOTable[j][5]==NIO[i]) //port should be equal to port number from IO file
                            {
                                if(aIOTable[j][0]==0) // Should be FX part
                                {
                                    fprintf(NewFile,"%d",(kIOTable[j][1]+2)); //write a cable number
                                    if(i!=TempCount-1)
                                    {
                                        fprintf(NewFile,", ");
                                    }
                                    FFlag=true;
                                    break;
                                }
                            }
                        }
                    }
                }
                else // if output
                {
                    //printf("OUT\n");
                    for(j=0;j<CCk;j++)
                    {
                        //printf("Moduls %d %d\n",ModuleIndexListFX[ModuleCounter],kIOTable[j][2]);
                        //printf("Ports %d %d\n",NIO[i],kIOTable[j][3]);
                        if(kIOTable[j][2]==ModuleIndexListFX[ModuleCounter]) //module to should be equal to module number
                        {
                            if(kIOTable[j][3]==NIO[i]) //port should be equal to port number from IO file
                            {
                                if(aIOTable[j][0]==0) // Should be FX part
                                {
                                    fprintf(NewFile,"%d",(kIOTable[j][1]+2)); //write a cable number
                                    if(i!=TempCount-1)
                                    {
                                        fprintf(NewFile,", ");
                                    }
                                    FFlag=true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(FFlag==false)
                {
                    fprintf(NewFile,"%d",IO[i]); // if it is input, we connect it to 0 bus. If it is input, we use a trash bus 1
                    if(i!=TempCount-1)
                    {
                        fprintf(NewFile,", ");
                    }
                }
            }
        }
    }

    //printf("InstrIO \n");

    return 1;
}
