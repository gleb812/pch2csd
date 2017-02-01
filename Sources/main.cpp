
#include <stdio.h>
#include <stdbool.h>
#include "menu.c"
#include "CableSort.c"
#include "II2IO.c"
#include "GenZakInit.c"
#include "OpenWrite.c"
#include "OpenTable.c"
#include "OWModule.c"
#include "GenInstrumentContent.c"
#include "GenInstrumentSpace.c"
#include "GenInstruments.c"
#include "NextField.c"
#include "CreatingNewFile.c"
#include "OpenPatchFile.c"
#include "ReadPD.c"
#include "ReadML.c"
#include "ReadMO.c"
#include "ReadPS.c"
#include "ReadCL.c"
#include "ReadMP.c"
#include "Welcome.c"
#include "ModuleTableCheck.c"
#include "TablesReader.c"
#include "SearchK2AModules.c"



FILE *NewFile;
FILE *ReadFile;
FILE *TempFile;
FILE *TempMapFile;
FILE *RecentFile;

int mcommand; // returned number of startmenu function !!!

const unsigned int L=40;
char TempFileName[L]="Modules\\"; // Path to Csound Instrs descriptions
char TempModuleMap[L]="Maps\\"; // Path to Map tables
char TempModuleIO[L]="IO\\"; // Path to modules IO tables

char NewFileName[50]="new.csd"; // Output csd file
char RecentFileName[50]="recent.txt"; // Txt-file with recent patch-file !!!
char PatchFileName[50]="Gleb2.pch2"; // Clavia NM2 patch file
char HeadFileName[50]="Heads/Header.txt"; // Csound header template
char EndingFileName[50]="Heads/Ending.txt"; // Csound tail template (score and XML ending)
char ModuleNamesTable[40]="Global/ModID2Name.txt"; // Table with

char NamesMapTables[6][256]; // Six Symbol Name Format - example CLA000

float Tables[128][128]; //Tables to map Nord parameters (in MIDI) to Csound parameters

unsigned int MapTablesVA[128][24]; //VA Module parameter types table (Module number 0-127)
unsigned int MapTablesFX[128][24]; //FX Module parameter types table (Module number 0-127)

unsigned int ParametersVA[128][64]; //Table with VA Module parameter values (Module number 0-127)
unsigned int ParametersFX[128][64]; //Table with FX Module parameter values (Module number 0-127)

unsigned int HiddenParametersVA[128]; //Table with VA Module hidden parameter value (Module number 0-127)
unsigned int HiddenParametersFX[128]; //Table with FX Module hidden parameter value (Module number 0-127)

bool HiddenFlagVA[128]; //Flag VA if Module has the hidden parameter (Module number 0-127)
bool HiddenFlagFX[128]; //Flag FX if Module has the hidden parameter (Module number 0-127)


unsigned int ParameterCountersVA[128]; //Table with VA Module overall number of parameters
unsigned int ParameterCountersFX[128]; //Table with FX Module overall number of parameters

unsigned int IOTable[256][9];   //Cables Table (inputs and outputs)
                                //0-location (VA or FX); 1-Cable number; 2-module from; 3-jack from;
                                //4-module to;  5-jack to; 6-color; 7-type; 8-a or k type in CSound
unsigned int aIOTable[256][6];  //Audio Cables Table (inputs and outputs)
                                //0-location (VA or FX); 1-Cable number; 2-module from; 3-jack from;
                                //4-module to;  5-jack to; 6-color; 7-type; 8-a or k type in CSound
unsigned int kIOTable[256][6];  //Control Cables Table (inputs and outputs)
                               //0-location (VA or FX); 1-Cable number; 2-module from; 3-jack from;
                                //4-module to;  5-jack to; 6-color; 7-type; 8-a or k type in CSound

unsigned int CableCounter=0; // Cable Counter

unsigned int CCa=0; // Audio Cable Counter (for csd file)
unsigned int CCk=0; // Control Cable Counter (for csd file)

unsigned int azakNumber=0;
unsigned int kzakNumber=0;

unsigned int ModuleCounter; // Module Counter

unsigned int PDposition; // PD section start
unsigned int PDlength; // PD section length
unsigned int MLposition; // etc ...
unsigned int MLlength;
unsigned int MOposition;
unsigned int MOlength;
unsigned int CLposition;
unsigned int CLlength;
unsigned int PSposition;
unsigned int PSlength;
unsigned int MPposition;
unsigned int MPlength;

unsigned int SoundCableCount=0; // Audio Cable Counter (for patch file)
unsigned int ControlCableCount=0; // Control Cable Counter (for patch file)
unsigned int OtherCableCount=0; // Other Cable Counter (for patch file)

unsigned int ModuleTypeList[1024]; // Module Type list (modules with the same type are listed only once)
unsigned int ModuleTypeCount=0; // Module type counter

unsigned int ModuleListVA[1024]; // VA module list
unsigned int ModuleListFX[1024]; // FX module list

bool seludoMAlertVA[1024]; // VA flag of eludoM
bool seludoMAlertFX[1024]; // FX flag of eludoM

unsigned int ModuleIndexListVA[1024]; // VA Module Index list
unsigned int ModuleIndexListFX[1024]; // FX Module Index list

unsigned int ModuleCountVA=0; // VA field module counter
unsigned int ModuleCountFX=0; // FX field module counter

bool VAFXFlag; //VA=1 FX=0

int main(void)
{
    Welcome();

    /************************************************************************/

    /************************************************************************/

    mcommand=menu();
    if(mcommand==0)
    {
        // Nord to CSound parameters mapping
        TablesReader();
        //OpenTable("Tables/CLA000.txt",1);
        //OpenTable("Tables/BUT002.txt",2);
        //OpenTable("Tables/CLAEXP",3);
        //OpenTable("Tables/BUT003.txt",4);

        CreatingNewFile(NewFileName);
        if(OpenPatchFile(PatchFileName)!=0)
        {
            if(ReadPD()!=0) // If patch header is OK
            {
                //Read module lists for VA & FX
                ReadML(PDposition+PDlength+3);
                ReadML(MLposition+MLlength+3);

                // Skip a Mistery Object
                ReadMO(MLposition+MLlength+3);

                //Read cable lists for VA & FX
                ReadCL(MOposition+MOlength+3);
                ReadCL(CLposition+CLlength+3);


                //Read patch parameters for VA & FX
                ReadPS(CLposition+CLlength+3);

                //Read module parameters for VA & FX
                //NOTICE!!! The oscillator wave type for some reason is not a parameter. It placed in module's description
                // So function ReadML should be revised
                ReadMP(PSposition+PSlength+3);
                ReadMP(MPposition+MPlength+3);

            }
        }

        OpenWrite(HeadFileName); // Start the new file from header template
        NextField(); // -
        II2IO4all(); // �������������� ������� �������� - �������������� ���������������� ����� � ���������� ��������
        CableSort(); // ������� � ���������������� �������� ����������� ��� �������� ����� �������������� �������

        SearchK2AModules();
        ModuleTableCheck();

        GenZakInit(); // zakinit creation
        NextField(); // -
        GenInstruments(); // CSound Instruments
        GenInstrumentSpace(); // Instr parameters
        NextField(); // -
        OpenWrite(EndingFileName); // file formating


    }
    if(mcommand==1)
    {
        printf("See you later!\n");
    }

/*
	char s[3];
	gets(s);
*/
	return 0;
}
