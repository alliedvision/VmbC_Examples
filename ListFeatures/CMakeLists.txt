cmake_minimum_required(VERSION 3.0)

project(ListFeatures LANGUAGES C)

find_package(Vmb MODULE REQUIRED COMPONENTS C)

set(COMMON_SOURCES
    ../Common/DiscoverGigECameras.c
    ../Common/DiscoverGigECameras.h
    ../Common/PrintVmbVersion.c
    ../Common/PrintVmbVersion.h
)

add_executable(ListFeatures
    main.c
    ListFeatures.c
    ListFeatures.h
    ${COMMON_SOURCES}
)

source_group(Common FILES ${COMMON_SOURCES})

target_link_libraries(ListFeatures PRIVATE Vmb::C)
set_target_properties(ListFeatures PROPERTIES
    C_STANDARD 11
    VS_DEBUGGER_ENVIRONMENT "PATH=${VMB_BINARY_DIRS};$ENV{PATH}")