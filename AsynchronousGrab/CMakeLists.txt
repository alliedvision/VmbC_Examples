cmake_minimum_required(VERSION 3.0)

project(ListFeatures LANGUAGES C)

find_package(Vmb MODULE REQUIRED COMPONENTS C ImageTransform)

set(COMMON_SOURCES
    ../Common/ErrorCodeToMessage.c
    ../Common/ErrorCodeToMessage.h
    ../Common/ListCameras.c
    ../Common/ListCameras.h
    ../Common/PrintVmbVersion.c
    ../Common/PrintVmbVersion.h
)

foreach(_SRC IN ITEMS
    Stdatomic
    Threads
)
    list(APPEND COMMON_SOURCES "../Common/Vmb${_SRC}.h")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../Common/Vmb${_SRC}_${CMAKE_SYSTEM_NAME}.c")
        list(APPEND COMMON_SOURCES
            "../Common/Vmb${_SRC}_${CMAKE_SYSTEM_NAME}.c"
            "../Common/Vmb${_SRC}_${CMAKE_SYSTEM_NAME}.h"
        )
    endif()
endforeach()

add_executable(AsynchronousGrab
    main.c
    AsynchronousGrab.c
    AsynchronousGrab.h
    ${COMMON_SOURCES}
)

source_group(Common FILES ${COMMON_SOURCES})

target_link_libraries(AsynchronousGrab PRIVATE Vmb::C Vmb::ImageTransform)
set_target_properties(AsynchronousGrab PROPERTIES
    C_STANDARD 11
    VS_DEBUGGER_ENVIRONMENT "PATH=${VMB_BINARY_DIRS};$ENV{PATH}")