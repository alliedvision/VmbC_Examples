cmake_minimum_required(VERSION 3.0)

project(AsynchronousGrab LANGUAGES C)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/vmb_cmake_prefix_paths.cmake")
    # read hardcoded package location information, if the example is still located in the original install location
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/vmb_cmake_prefix_paths.cmake)
endif()

find_package(Vmb REQUIRED COMPONENTS C ImageTransform NAMES Vmb VmbC VmbCPP VmbImageTransform)

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
if (UNIX)
    target_link_libraries(AsynchronousGrab_VmbC PRIVATE pthread)
endif()

set_target_properties(AsynchronousGrab_VmbC PROPERTIES
    C_STANDARD 11
    VS_DEBUGGER_ENVIRONMENT "PATH=${VMB_BINARY_DIRS};$ENV{PATH}"
)
