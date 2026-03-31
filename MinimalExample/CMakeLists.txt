cmake_minimum_required(VERSION 3.16)

project(MinimalExample VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(QFluent REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        widget.cpp
        widget.h
        widget.ui
        AcrylicWidget.cpp
        AcrylicWidget.h
)

# Application icon
if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")
    list(APPEND PROJECT_SOURCES ${APP_ICON_RESOURCE_WINDOWS})
elseif(APPLE)
    set(APP_ICON_MACOS "${CMAKE_CURRENT_SOURCE_DIR}/app.icns")
    set_source_files_properties(${APP_ICON_MACOS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    list(APPEND PROJECT_SOURCES ${APP_ICON_MACOS})
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MinimalExample
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MinimalExample APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MinimalExample SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MinimalExample
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(MinimalExample PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(MinimalExample PRIVATE QFluent::QFluent)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.MinimalExample)
endif()
set_target_properties(MinimalExample PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MinimalExample)
endif()
