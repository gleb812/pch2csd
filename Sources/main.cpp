#include <iostream>
#include <string>
#include <stdio.h>
#include <stdbool.h>
#include <map>

#include "Config.h"
#include "Utils.h"
#include "Utils.c"

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
#include "AddConverters.c"
#include "ModuleTypeListSort.c"

FILE *NewFile;
FILE *ReadFile;
FILE *TempFile;
FILE *TempMapFile;
FILE *RecentFile;

int mcommand; // returned number of startmenu function !!!

//const unsigned int L = 1024;
//char TempFileName[L] = "Modules\\"; // Path to Csound Instrs descriptions
//char TempModuleMap[L] = "Maps\\"; // Path to Map tables
//char TempModuleIO[L] = "IO\\"; // Path to modules IO tables

//char NewFileName[1024] = "new.csd"; // Output csd file
//char RecentFileName[1024] = "recent.txt"; // Txt-file with recent patch-file !!!
//char PatchFileName[1024] = "Gleb2.pch2"; // Clavia NM2 patch file
//char HeadFileName[1024] = "Heads/Header.txt"; // Csound header template
//char EndingFileName[1024] = "Heads/Ending.txt"; // Csound tail template (score and XML ending)
//char ModuleNamesTable[1024] = "Global/ModID2Name.txt"; // Table with

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

unsigned int CableCounter = 0; // Cable Counter

unsigned int CCa = 0; // Audio Cable Counter (for csd file)
unsigned int CCk = 0; // Control Cable Counter (for csd file)

unsigned int azakNumber = 0;
unsigned int kzakNumber = 0;

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

unsigned int SoundCableCount = 0; // Audio Cable Counter (for patch file)
unsigned int ControlCableCount = 0; // Control Cable Counter (for patch file)
unsigned int OtherCableCount = 0; // Other Cable Counter (for patch file)

unsigned int ModuleTypeList[1024]; // Module Type list (modules with the same type are listed only once)
unsigned int ModuleTypeCount = 0; // Module type counter

unsigned int ModuleListVA[1024]; // VA module list
unsigned int ModuleListFX[1024]; // FX module list

bool seludoMAlertVA[1024]; // VA flag of eludoM
bool seludoMAlertFX[1024]; // FX flag of eludoM

unsigned int ModuleIndexListVA[1024]; // VA Module Index list
unsigned int ModuleIndexListFX[1024]; // FX Module Index list

unsigned int ModuleCountVA = 0; // VA field module counter
unsigned int ModuleCountFX = 0; // FX field module counter

bool a2kFlag = false;
bool k2aFlag = false;

bool VAFXFlag; //VA=1 FX=0

unsigned int i;

using namespace std;

void RunAll(map<string, string> &params) {
    TablesReader();

    CreatingNewFile(params["file_out"].c_str());
    if (OpenPatchFile(params["file_in"].c_str()) != 0) {
        if (ReadPD() != 0) // If patch header is OK
        {
            //Read module lists for VA & FX
            ReadML(PDposition + PDlength + 3);
            ReadML(MLposition + MLlength + 3);

            // Skip a Mistery Object
            ReadMO(MLposition + MLlength + 3);

            //Read cable lists for VA & FX
            ReadCL(MOposition + MOlength + 3);
            ReadCL(CLposition + CLlength + 3);


            //Read patch parameters for VA & FX
            ReadPS(CLposition + CLlength + 3);

            //Read module parameters for VA & FX
            //NOTICE!!! The oscillator wave type for some reason is not a parameter. It placed in module's description
            // So function ReadML should be revised

            ReadMP(PSposition + PSlength + 3);
            ReadMP(MPposition + MPlength + 3);


        }
    }


    OpenWrite(NM_FileFooter); // Start the new file from header template
    NextField(); // -
    II2IO4all();
    CableSort();


    SearchK2AModules();

    ModuleTableCheck();

    AddConverters();

    ModuleTypeListSort();

    GenZakInit(); // zakinit creation
    NextField(); // -
    GenInstruments(); // CSound Instruments
    GenInstrumentSpace(); // Instr parameters
    NextField(); // -
    OpenWrite(NM_FileFooter); // file formating
}

void ParseArgs(int argc, char *argv[], map<string, string> &params) {
    if (argc < 2) {
        Help();
        exit(0);
    }

    params["file_in"] = string(argv[1]);
    params["file_out"] = params["file_in"] + ".csd";

    for (int i = 2; i < argc; i++) {
        if (string(argv[i]) == "-d" & (i + 1) < argc) {
            i++;
            auto arg = string(argv[i]);
            if (arg[arg.length()] != '/') {
                arg.push_back('/');
            }
            params["dir_data"] = arg;
        }
    }

    if (params.find("dir_data") == params.end()) {
        params["dir_data"] = "Data/";
    }

    params["dir_ft"] = params["dir_data"] + "Function_Tables/";
    params["dir_global"] = params["dir_data"] + "Function_Global/";
    params["dir_heads"] = params["dir_data"] + "Heads/";
    params["dir_io"] = params["dir_data"] + "IO/";
    params["dir_maps"] = params["dir_data"] + "Maps/";
    params["dir_modules"] = params["dir_data"] + "Modules/";
    params["dir_k2a"] = params["dir_data"] + "RulesK2A/";
    params["dir_seludom"] = params["dir_data"] + "seludoM/";
    params["dir_tables"] = params["dir_data"] + "Tables/";
    params["dir_test"] = params["dir_data"] + "Test/";

    params["file_header"] = params["dir_heads"] + "Header.txt";
    params["file_footer"] = params["dir_heads"] + "Ending.txt";
    params["file_modID2Name"] = params["dir_global"] + "ModID2Name.txt";

}

void Configure(map<string, string> &params) {
    NM_DirData = params["dir_data"].c_str();
    NM_DirFunctionTables = params["dir_ft"].c_str();
    NM_DirGlobal = params["dir_global"].c_str();
    NM_DirHeads = params["dir_heads"].c_str();
    NM_DirIO = params["dir_io"].c_str();
    NM_DirMaps = params["dir_maps"].c_str();
    NM_DirModules = params["dir_modules"].c_str();
    NM_DirK2A = params["dir_k2a"].c_str();
    NM_DirSeludoM = params["dir_seludom"].c_str();
    NM_DirTables = params["dir_tables"].c_str();
    NM_DirTest = params["dir_test"].c_str();

    NM_FileHeader = params["file_header"].c_str();
    NM_FileFooter = params["file_footer"].c_str();
    NM_FileModID2Name = params["file_modID2Name"].c_str();
}

int main(int argc, char *argv[]) {
    auto params = map<string, string>();

    ParseArgs(argc, argv, params);
    Configure(params);
    RunAll(params);

    return 0;
}
