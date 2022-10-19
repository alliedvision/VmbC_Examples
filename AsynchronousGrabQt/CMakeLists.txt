#[===[
  Copyright (C) 2021-2022 Allied Vision Technologies.  All Rights Reserved.
 
  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

 -----------------------------------------------------------------------------
  CMake project for the AsynchronousGrab Qt example using the Vmb C API
 -----------------------------------------------------------------------------

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
  NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
]===]

cmake_minimum_required(VERSION 3.0)

project(AsynchronousGrabQt)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/vmb_cmake_prefix_paths.cmake")
    # read hardcoded package location information, if the example is still located in the original install location
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/vmb_cmake_prefix_paths.cmake)
endif()

find_package(Vmb REQUIRED COMPONENTS C ImageTransform)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(SOURCES)
set(HEADERS)

# add source+header combination in this dir
foreach(_FILE_NAME IN ITEMS
    AcquisitionManager
    ApiController
    Image
    ImageTranscoder
    LogEntryListModel
    ModuleData
    ModuleTreeModel
    VmbException
    VmbLibraryLifetime
)
    list(APPEND SOURCES "${_FILE_NAME}.cpp")
    list(APPEND HEADERS "${_FILE_NAME}.h")
endforeach()

# add sources without corresponding header
list(APPEND SOURCES
    main.cpp
)

# add headers without corresponding source
list(APPEND HEADERS
    support/NotNull.h
    LogEntry.h
)

if(NOT WIN32)
    list(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,'\$ORIGIN'")
endif()

add_executable(AsynchronousGrabQt_VmbC
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(AsynchronousGrabQt_VmbC PRIVATE Qt5::Widgets Vmb::C Vmb::ImageTransform)
if (UNIX)
    target_link_libraries(AsynchronousGrabQt_VmbC PRIVATE pthread)
endif()

target_include_directories(AsynchronousGrabQt_VmbC PRIVATE . ${CMAKE_CURRENT_BINARY_DIR})

# find out binary dir of qt
get_filename_component(_VMB_QT_BIN_DIR ${Qt5_DIR} DIRECTORY)
get_filename_component(_VMB_QT_BIN_DIR ${_VMB_QT_BIN_DIR} DIRECTORY)
get_filename_component(_VMB_QT_BIN_DIR ${_VMB_QT_BIN_DIR} DIRECTORY)

set_target_properties(AsynchronousGrabQt_VmbC
    PROPERTIES
        CXX_STANDARD 11
        VS_DEBUGGER_ENVIRONMENT "PATH=${VMB_BINARY_DIRS};${_VMB_QT_BIN_DIR}/bin;$ENV{PATH}"
)

# provide target_sources that suits our needs in case it's not yet built in 
if (NOT COMMAND target_sources)
    macro(target_sources TARGET VISIBILITY SOURCE_FILE)
        foreach(_SRC IN ITEMS ${SOURCE_FILE} ${ARGN})
            set_property(TARGET ${TARGET} APPEND PROPERTY SOURCES ${_SRC})
        endforeach()
    endmacro()
endif()

###############################################################################
# qt source generation ########################################################
###############################################################################

set(GENERATED_UI_SOURCES)
set(MOC_HEADERS)
set(UI_FILES)
set(UI_SOURCES)

# uic generation
foreach(_UI_SRC IN ITEMS
    AsynchronousGrabGui
)
    set(_SRC UI/res/${_UI_SRC}.ui)
    list(APPEND UI_FILES ${_SRC})
    qt5_wrap_ui(GENERATED_UI_SOURCES ${_SRC})
endforeach()

# moc compilation
foreach(_MOC_SRC IN ITEMS
    MainWindow
    ImageLabel
)
    set(_SRC UI/${_MOC_SRC}.h)
    list(APPEND MOC_HEADERS ${_SRC})
    list(APPEND UI_SOURCES UI/${_MOC_SRC}.cpp)
    qt5_wrap_cpp(GENERATED_UI_SOURCES ${_SRC} TARGET AsynchronousGrabQt_VmbC)
endforeach()

list(APPEND UI_SOURCES imagelabel.h)

set_source_files_properties(${GENERATED_UI_SOURCES} PROPERTIES
    GENERATED True
)

target_sources(AsynchronousGrabQt_VmbC PRIVATE
    ${GENERATED_UI_SOURCES}
    ${MOC_HEADERS}
    ${UI_FILES}
    ${UI_SOURCES}
)

if(UNIX)
  target_link_options(AsynchronousGrabQt_VmbC PUBLIC -no-pie)
endif()

# source groups for IDE
source_group(Resources FILES ${UI_FILES})
source_group("Source Files\\UI" FILES ${UI_SOURCES})
source_group("Header Files\\UI" FILES ${MOC_HEADERS})
source_group("Generated" FILES ${GENERATED_UI_SOURCES})

# use executable as default startup project in Visual Studio
set_property(DIRECTORY . PROPERTY VS_STARTUP_PROJECT AsynchronousGrabQt_VmbC)
