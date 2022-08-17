cmake_minimum_required(VERSION 3.0)

project(PersistentIp LANGUAGES C)

find_package(Vmb MODULE REQUIRED COMPONENTS C)

if(NOT TARGET VmbCExamplesCommon)
    add_subdirectory(../Common VmbCExamplesCommon_build)
endif()

add_executable(PersistentIp_VmbC
    main.c
    PersistentIp.c
    PersistentIp.h
    PersistentIpProg.c
    PersistentIpProg.h
    ${COMMON_SOURCES}
)

source_group(Common FILES ${COMMON_SOURCES})

target_compile_definitions(PersistentIp_VmbC PRIVATE _LITTLE_ENDIAN)

target_link_libraries(PersistentIp_VmbC PRIVATE Vmb::C VmbCExamplesCommon)

if (WIN32)
    target_link_libraries(PersistentIp_VmbC PRIVATE Ws2_32)
endif()

set_target_properties(PersistentIp_VmbC PROPERTIES
    C_STANDARD 11
    VS_DEBUGGER_ENVIRONMENT "PATH=${VMB_BINARY_DIRS};$ENV{PATH}")