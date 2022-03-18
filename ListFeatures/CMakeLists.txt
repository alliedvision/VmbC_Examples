cmake_minimum_required(VERSION 3.0)

project(ListFeatures LANGUAGES C)

find_package(Vmb MODULE REQUIRED COMPONENTS C)

if(NOT TARGET VmbCExamplesCommon)
    add_subdirectory(../Common VmbCExamplesCommon_build)
endif()

add_executable(ListFeatures_VmbC
    main.c
    ListFeatures.c
    ListFeatures.h
    ${COMMON_SOURCES}
)

source_group(Common FILES ${COMMON_SOURCES})

target_link_libraries(ListFeatures_VmbC PRIVATE Vmb::C VmbCExamplesCommon)
set_target_properties(ListFeatures_VmbC PROPERTIES
    C_STANDARD 11
    VS_DEBUGGER_ENVIRONMENT "PATH=${VMB_BINARY_DIRS};$ENV{PATH}")