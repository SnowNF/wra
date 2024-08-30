cmake_minimum_required(VERSION 3.5)

project(control-panel VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Xml)

file(GLOB WIDGETS widgets/*)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ${WIDGETS}
        utils/Utils.cpp
        utils/Utils.h
        utils/XmlHelper.cpp
        utils/XmlHelper.h
)

qt_add_executable(control-panel
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
)

target_link_libraries(control-panel PRIVATE Qt6::Widgets Qt6::Xml)

target_include_directories(control-panel PRIVATE . utils)

include(GNUInstallDirs)
install(TARGETS control-panel
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(control-panel)
