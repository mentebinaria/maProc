cmake_minimum_required(VERSION 3.5)

project(maProc VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(OPTIMIZATIONS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(CMAKE_CXX_FLAGS " ")

if(OPTIMIZATIONS)
    set(CMAKE_CXX_FLAGS "-ftree-vectorize -march=native -fdelete-dead-exceptions -mavx -fno-stack-protector -march=native -O2 -Os -fdelete-null-pointer-checks")
    set(DEBUGGER OFF)
endif()

if(DEBUGGER)
    set(CMAKE_CXX_FLAGS "-g -gdwarf -gdwarf-version")
endif()

set(PROJECT_SOURCES

    src/include/elf/relf.cpp
    src/include/elf/relf.hpp

    src/main.cpp
    src/mainwindow.cpp

    src/include/ps/ps.hpp
    src/include/ps/ps.cpp

    src/include/memory/arena.hpp
    src/include/memory/arena.cpp

    src/include/fd/filedescriptor.hpp
    src/include/fd/filedescriptor.cpp

    src/include/map/pmap.hpp
    src/include/map/pmap.cpp

    src/include/map/proclib.hpp
    src/include/map/proclib.cpp

    src/qhexview.cpp
    src/qhexview.hpp

    src/mainwindow.hpp
    src/gui/mainwindow.ui
    src/gui/about.ui

    src/dirwindow.hpp
    src/dirwindow.cpp
    src/gui/dirwindow.ui
)
add_executable(maProc
    ${PROJECT_SOURCES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(maProc
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(maProc PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(maProc PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(maProc)
endif()