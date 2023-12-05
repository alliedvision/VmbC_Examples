cmake_minimum_required(VERSION 3.0)

project(VmbCExamples)

set(VMB_C_EXAMPLES_LIST IGNORE CACHE STRING
    "a semicolon separated list of examples to configure; takes precedence over other settings to enable or disable examples"
)

set(VMB_C_ALL_EXAMPLES)

# function takes the directory and optionally aliases other than the directory the example can be refered by
function(vmb_c_example DIRECTORY)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}")
        set(VMB_C_EXAMPLE_IGNORE_${DIRECTORY} False CACHE BOOL "Ignore the ${DIRECTORY} example; VMB_C_EXAMPLES_LIST takes precedence over this property")
        foreach(_ALIAS IN LISTS DIRECTORY ARGN)
            set(VMB_C_ALIAS_${_ALIAS} ${DIRECTORY} PARENT_SCOPE)
        endforeach()
        if (NOT VMB_C_EXAMPLE_IGNORE_${DIRECTORY})
            set(VMB_C_ALL_EXAMPLES ${VMB_C_ALL_EXAMPLES} ${DIRECTORY} PARENT_SCOPE)
        endif()
    endif()
endfunction()

# attempt to locate Qt to decide, if we want to add the Qt example
find_package(Qt5 COMPONENTS Widgets)

# Actual examples list
vmb_c_example(AsynchronousGrab)
vmb_c_example(ListFeatures FeatureList FeaturesList)
vmb_c_example(ListCameras)
vmb_c_example(ChunkAccess)
vmb_c_example(ConfigIp)
vmb_c_example(ForceIp)
vmb_c_example(ActionCommands)
vmb_c_example(EventHandling)

if(Qt5_FOUND)
    vmb_c_example(AsynchronousGrabQt Qt)
endif()

# overwrite list of examples set based on individual ignores
if(VMB_C_EXAMPLES_LIST)
    set(VMB_C_ALL_EXAMPLES)
    foreach(_EXAMPLE IN LISTS VMB_C_EXAMPLES_LIST)
        set(_DIR ${VMB_C_ALIAS_${_EXAMPLE}})
        if (NOT _DIR)
            message(FATAL_ERROR "${_EXAMPLE} found in VMB_C_EXAMPLES_LIST is not a known example")
        else()
            set(VMB_C_ALL_EXAMPLES ${VMB_C_ALL_EXAMPLES} ${_DIR})
        endif()
    endforeach()
endif()

# finally add the necessary subdirectories
list(REMOVE_DUPLICATES VMB_C_ALL_EXAMPLES)

if (NOT VMB_C_ALL_EXAMPLES)
    message(FATAL_ERROR "no active examples")
endif()

foreach(_EXAMPLE IN LISTS VMB_C_ALL_EXAMPLES)
    add_subdirectory(${_EXAMPLE})
endforeach()

# collect all targets for installation
function(get_all_targets var)
    set(targets)
    get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
    set(${var} ${targets} PARENT_SCOPE)
endfunction()

macro(get_all_targets_recursive targets dir)
    get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirectories})
        get_all_targets_recursive(${targets} ${subdir})
    endforeach()

    get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    list(APPEND ${targets} ${current_targets})
endmacro()

get_all_targets(ALL_TARGETS)
list(REMOVE_ITEM ALL_TARGETS "VmbCExamplesCommon")
install(TARGETS ${ALL_TARGETS} DESTINATION bin)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt" DESTINATION src)

# exclude platform depending build files
if(WIN32)
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Common" DESTINATION src)
    foreach(_EXAMPLE IN LISTS VMB_C_ALL_EXAMPLES)
        install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_EXAMPLE}" DESTINATION src
            PATTERN "*.xcodeproj" EXCLUDE
            PATTERN "*.xcworkspace" EXCLUDE
        )
    endforeach()
else()
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Common" DESTINATION src
        PATTERN "build_vs" EXCLUDE
    )
    if(APPLE)
        foreach(_EXAMPLE IN LISTS VMB_C_ALL_EXAMPLES)
            install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_EXAMPLE}" DESTINATION src
                PATTERN "*.sln" EXCLUDE
                PATTERN "*.vcxproj*" EXCLUDE
            )
        endforeach()
    else() # Linux
        foreach(_EXAMPLE IN LISTS VMB_C_ALL_EXAMPLES)
            install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_EXAMPLE}" DESTINATION src
                PATTERN "*.sln" EXCLUDE
                PATTERN "*.vcxproj*" EXCLUDE
                PATTERN "*.xcodeproj" EXCLUDE
                PATTERN "*.xcworkspace" EXCLUDE
            )
        endforeach()
    endif()
endif()
