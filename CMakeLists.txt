cmake_minimum_required(VERSION 3.5)

project(Qt-threads VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenCV_DIR "F:\\Files\\libs\\opencv\\build-sources")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
message(STATUS "Cmake moduels " ${CMAKE_MODULE_PATH})
find_package(OpenCV REQUIRED)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    threadworker.cpp

)

set(PROJECT_HEADERS
    threadworker.h
    mainwindow.h)

set(PROJECT_FORMS
    mainwindow.ui)

add_executable(Qt-threads ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${PROJECT_FORMS})

target_link_libraries(Qt-threads PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Qt-threads PRIVATE ${OpenCV_LIBS})
message(STATUS "Include dirs: " ${OpenCV_INCLUDE_DIRS})
target_include_directories(Qt-threads PRIVATE ${OpenCV_INCLUDE_DIRS})

set_target_properties(Qt-threads PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Qt-threads)
endif()
