cmake_minimum_required(VERSION 3.6)
project(pch2csd)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
    AddConverters.c
    CableSort.c
    CreatingNewFile.c
    GenInstrumentContent.c
    GenInstruments.c
    GenInstrumentSpace.c
    GenZakInit.c
    II2IO.c
    main.cpp
    menu.c
    ModuleTableCheck.c
    ModuleTypeListSort.c
    NextField.c
    OpenPatchFile.c
    OpenTable.c
    OpenWrite.c
    OWModule.c
    ReadCL.c
    ReadML.c
    ReadMO.c
    ReadMP.c
    ReadPD.c
    ReadPS.c
    SearchK2AModules.c
    TablesReader.c
    Welcome.c)

add_executable(pch2csd ${SOURCE_FILES})