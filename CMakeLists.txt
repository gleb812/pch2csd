cmake_minimum_required(VERSION 3.6.0)
project(pch2csd)

set(SOURCE_FILES
        Sources/CableSort.c Sources/CreatingNewFile.c Sources/GenInstrumentContent.c Sources/GenInstruments.c
        Sources/GenInstrumentSpace.c Sources/GenZakInit.c Sources/II2IO.c Sources/main.cpp Sources/menu.c
        Sources/ModuleTableCheck.c Sources/NextField.c Sources/OpenPatchFile.c Sources/OpenTable.c Sources/OpenWrite.c
        Sources/OWModule.c Sources/ReadCL.c Sources/ReadML.c Sources/ReadMO.c Sources/ReadMP.c Sources/ReadPD.c
        Sources/ReadPS.c Sources/TablesReader.c Sources/Welcome.c Sources/Util.c)


set(CMAKE_BUILD_TYPE Release)

add_executable(pch2csd ${SOURCE_FILES})

#find_library(libc c)
#target_link_libraries(pch2csd ${libc})

set_target_properties(
        pch2csd
        PROPERTIES
        LINK_FLAGS "-v"
)


add_custom_command(TARGET pch2csd PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Resources/Templates ${CMAKE_BINARY_DIR})
add_custom_command(TARGET pch2csd PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Resources/Patches ${CMAKE_BINARY_DIR}/Patches)
