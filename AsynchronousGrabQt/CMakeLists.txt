#
# Copyright (C) 2021 Allied Vision Technologies.  All Rights Reserved.
# 
#  Redistribution of this file, in original or modified form, without
#  prior written consent of Allied Vision Technologies is prohibited.
#
# -----------------------------------------------------------------------------
#  CMake project for the AsynchronousGrab Qt example using the Vmb C API
# -----------------------------------------------------------------------------
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
#  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
#  NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
#  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
#  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
#  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
#  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

cmake_minimum_required(VERSION 3.0)

project(AsynchronousGrabQtC)

find_package(Vmb MODULE REQUIRED COMPONENTS C ImageTransform)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(AsynchronousGrabQtC

    # sources
    ApiController.cpp
    LogEntry.cpp
    main.cpp
    ModuleData.cpp
    ModuleTreeModel.cpp

    # headers
    ApiController.h
    ApiCallResult.h
    LogEntry.h
    ModuleData.h
    ModuleTreeModel.h
)

target_link_libraries(AsynchronousGrabQtC PRIVATE Qt5::Widgets Vmb::C Vmb::ImageTransform)
target_include_directories(AsynchronousGrabQtC PRIVATE . ${CMAKE_CURRENT_BINARY_DIR})

# find out binary dir of qt
get_filename_component(_VMB_QT_BIN_DIR ${Qt5_DIR} DIRECTORY)
get_filename_component(_VMB_QT_BIN_DIR ${_VMB_QT_BIN_DIR} DIRECTORY)
get_filename_component(_VMB_QT_BIN_DIR ${_VMB_QT_BIN_DIR} DIRECTORY)

set_target_properties(AsynchronousGrabQtC
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
set(UI_RESOURCES)
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
)
    set(_SRC UI/${_MOC_SRC}.h)
    list(APPEND MOC_HEADERS ${_SRC})
    list(APPEND UI_SOURCES UI/${_MOC_SRC}.cpp)
    qt5_wrap_cpp(GENERATED_UI_SOURCES ${_SRC} TARGET AsynchronousGrabQtC)
endforeach()

# rcc code generation
foreach(_RCC_SRC IN ITEMS
    AsynchronousGrab
)
    set(_SRC UI/res/${_RCC_SRC}.qrc)
    list(APPEND UI_RESOURCES ${_SRC})
    qt5_add_resources(GENERATED_UI_SOURCES ${_SRC})
endforeach()

set_source_files_properties(${GENERATED_UI_SOURCES}
    PROPERTIES
        GENERATED True
)

target_sources(AsynchronousGrabQtC PRIVATE
    ${GENERATED_UI_SOURCES}
    ${MOC_HEADERS}
    ${UI_RESOURCES}
    ${UI_FILES}
    ${UI_SOURCES}
)

# source groups for IDE
source_group(Resources FILES ${UI_RESOURCES} ${UI_FILES})
source_group("Source Files\\UI" FILES ${UI_SOURCES})
source_group("Header Files\\UI" FILES ${MOC_HEADERS})
source_group("Generated" FILES ${GENERATED_UI_SOURCES})