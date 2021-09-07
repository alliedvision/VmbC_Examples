cmake_minimum_required(VERSION 3.0)

project(ListFeatures LANGUAGES C)

find_package(Vmb MODULE REQUIRED COMPONENTS C)

set(COMMON_SOURCES
    ../Common/ListCameras.c
    ../Common/ListCameras.h
    ../Common/ListInterfaces.c
    ../Common/ListInterfaces.h
    ../Common/ListTransportLayers.c
    ../Common/ListTransportLayers.h
    ../Common/PrintVmbVersion.c
    ../Common/PrintVmbVersion.h
    ../Common/TransportLayerTypeToString.c
    ../Common/TransportLayerTypeToString.h
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