cmake_minimum_required(VERSION 3.0)

project(ListFeatures LANGUAGES C)

find_package(Vmb MODULE REQUIRED COMPONENTS C ImageTransform)

if(NOT TARGET VmbCExamplesCommon)
    add_subdirectory(../Common VmbCExamplesCommon_build)
endif()

add_executable(AsynchronousGrab_VmbC
    main.c
    AsynchronousGrab.c
    AsynchronousGrab.h
    ${COMMON_SOURCES}
)

target_link_libraries(AsynchronousGrab_VmbC PRIVATE Vmb::C Vmb::ImageTransform VmbCExamplesCommon)
set_target_properties(AsynchronousGrab_VmbC PROPERTIES
    C_STANDARD 11
    VS_DEBUGGER_ENVIRONMENT "PATH=${VMB_BINARY_DIRS};$ENV{PATH}")