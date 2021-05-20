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

project(AsynchronousGrabQt)

find_package(Vmb MODULE REQUIRED COMPONENTS C ImageTransform)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

###############################################################################
# collect source files ########################################################
###############################################################################
set(UI_RESOURCES
    UI/res/AsynchronousGrab.qrc
    UI/res/MainWindow.ui
)

set(UI_SOURCES
    UI/MainWindow.cpp
)

set(UI_HEADERS
    UI/MainWindow.h
)

set(SOURCES
    ApiController.cpp
)

set(HEADERS
    ApiController.h
)

source_group(Resources FILES ${UI_RESOURCES})
source_group("Source Files\\UI" FILES ${UI_SOURCES})
source_group("Header Files\\UI" FILES ${UI_HEADERS})

add_executable(AsynchronousGrabQt
    main.cpp
    ${UI_RESOURCES}
    ${UI_SOURCES}
    ${UI_HEADERS}
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(AsynchronousGrabQt PRIVATE Qt5::Widgets Vmb::C Vmb::ImageTransform)
target_include_directories(AsynchronousGrabQt PRIVATE .)

set_target_properties(AsynchronousGrabQt
    PROPERTIES
        AUTOMOC True
        AUTOUIC True
        AUTORCC True
        CXX_STANDARD 11
)