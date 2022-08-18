cmake_minimum_required(VERSION 3.0)

project(ChunkAccess LANGUAGES C)

find_package(Vmb MODULE REQUIRED COMPONENTS C)

if(NOT TARGET VmbCExamplesCommon)
    add_subdirectory(../Common VmbCExamplesCommon_build)
endif()

add_executable(ChunkAccess_VmbC
    main.c
    ChunkAccessProg.c
    ChunkAccessProg.h
    ${COMMON_SOURCES}
)

source_group(Common FILES ${COMMON_SOURCES})

target_link_libraries(ChunkAccess_VmbC PRIVATE Vmb::C VmbCExamplesCommon)
set_target_properties(ChunkAccess_VmbC PROPERTIES
    C_STANDARD 11
    VS_DEBUGGER_ENVIRONMENT "PATH=${VMB_BINARY_DIRS};$ENV{PATH}")