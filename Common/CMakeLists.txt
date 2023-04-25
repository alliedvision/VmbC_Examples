project(ListFeatures LANGUAGES C)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/vmb_cmake_prefix_paths.cmake")
    # read hardcoded package location information, if the example is still located in the original install location
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/vmb_cmake_prefix_paths.cmake)
endif()

find_package(Vmb REQUIRED COMPONENTS C ImageTransform NAMES Vmb VmbC VmbCPP VmbImageTransform)

set(SOURCES)
set(HEADERS
    include/VmbCExamplesCommon/ArrayAlloc.h
)

#OS dependent sources
foreach(SRC IN ITEMS
    VmbStdatomic
    VmbThreads
)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SRC}_${CMAKE_SYSTEM_NAME}.c")
        list(APPEND SOURCES "${SRC}_${CMAKE_SYSTEM_NAME}.c")
        list(APPEND HEADERS "include/VmbCExamplesCommon/${SRC}_${CMAKE_SYSTEM_NAME}.h")
    endif()

    list(APPEND HEADERS include/VmbCExamplesCommon/${SRC}.h)
endforeach()

set(SOURCES_WITH_HEADERS
    AccessModeToString
    ErrorCodeToMessage
    IpAddressToHostByteOrderedInt
    ListCameras
    ListInterfaces
    ListTransportLayers
    PrintVmbVersion
    TransportLayerTypeToString
)

foreach(SRC IN LISTS SOURCES_WITH_HEADERS)
    list(APPEND SOURCES ${SRC}.c)
    list(APPEND HEADERS include/VmbCExamplesCommon/${SRC}.h)
endforeach()

add_library(VmbCExamplesCommon STATIC
    ${SOURCES}
    ${HEADERS}
)

set_target_properties(VmbCExamplesCommon PROPERTIES
    C_STANDARD 11
)

target_compile_definitions(VmbCExamplesCommon PRIVATE _LITTLE_ENDIAN)

target_include_directories(VmbCExamplesCommon PUBLIC
    include
    $<TARGET_PROPERTY:Vmb::C,INTERFACE_INCLUDE_DIRECTORIES>
)
