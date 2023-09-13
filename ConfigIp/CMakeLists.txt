cmake_minimum_required(VERSION 3.0)

project(ConfigIp LANGUAGES C)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/vmb_cmake_prefix_paths.cmake")
    # read hardcoded package location information, if the example is still located in the original install location
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/vmb_cmake_prefix_paths.cmake)
endif()

find_package(Vmb REQUIRED COMPONENTS C NAMES Vmb VmbC VmbCPP VmbImageTransform)

if(NOT TARGET VmbCExamplesCommon)
    add_subdirectory(../Common VmbCExamplesCommon_build)
endif()

add_executable(ConfigIp_VmbC
    main.c
    ConfigIp.c
    ConfigIp.h
    ConfigIpProg.c
    ConfigIpProg.h
    ${COMMON_SOURCES}
)

source_group(Common FILES ${COMMON_SOURCES})

target_compile_definitions(ConfigIp_VmbC PRIVATE _LITTLE_ENDIAN)

target_link_libraries(ConfigIp_VmbC PRIVATE Vmb::C VmbCExamplesCommon)

if (WIN32)
    target_link_libraries(ConfigIp_VmbC PRIVATE Ws2_32)
endif()

set_target_properties(ConfigIp_VmbC PROPERTIES
    C_STANDARD 11
    VS_DEBUGGER_ENVIRONMENT "PATH=${VMB_BINARY_DIRS};$ENV{PATH}"
)
