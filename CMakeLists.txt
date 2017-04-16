cmake_minimum_required(VERSION 2.7)
project(pch2csd)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        Sources/AddConverters.c
        Sources/CableSort.c
        Sources/CreatingNewFile.c
        Sources/GenInstrumentContent.c
        Sources/GenInstruments.c
        Sources/GenInstrumentSpace.c
        Sources/GenZakInit.c
        Sources/II2IO.c
        Sources/main.cpp
        Sources/ModuleTableCheck.c
        Sources/ModuleTypeListSort.c
        Sources/NextField.c
        Sources/OpenPatchFile.c
        Sources/OpenTable.c
        Sources/OpenWrite.c
        Sources/OWModule.c
        Sources/ReadCL.c
        Sources/ReadML.c
        Sources/ReadMO.c
        Sources/ReadMP.c
        Sources/ReadPD.c
        Sources/ReadPS.c
        Sources/SearchK2AModules.c
        Sources/TablesReader.c
        Sources/Welcome.c Sources/Utils.h Sources/Config.h Sources/Utils.c)

add_executable(pch2csd ${SOURCE_FILES})