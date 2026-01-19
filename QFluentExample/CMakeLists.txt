cmake_minimum_required(VERSION 3.5)
project(QFluentExample LANGUAGES CXX)

# 支持Qt5和Qt6
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Svg Xml)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(BUILD_QWINDOWKIT OFF)

file(GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.cpp
    src/*.h
)
set(CPP_SOURCES "")
set(HEADER_SOURCES "")
foreach(file ${SOURCES})
    if(file MATCHES "\\.cpp$")
        list(APPEND CPP_SOURCES ${file})
    elseif(file MATCHES "\\.h$")
        list(APPEND HEADER_SOURCES ${file})
    endif()
endforeach()

file(GLOB_RECURSE AUTO_RESOURCE_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    res/*.png res/*.jpg res/*.jpeg res/*.gif res/*.svg res/*.ico res/*.ttf res/*.qss
)

# 资源处理：兼容Qt5和Qt6
if(${QT_VERSION_MAJOR} EQUAL 6)
    add_executable(QFluentExample
        WIN32 MACOSX_BUNDLE
        ${CPP_SOURCES}
        ${HEADER_SOURCES}
        version.rc
    )
    qt_add_resources(QFluentExample "QFluentExample"
        PREFIX "/"
        FILES ${AUTO_RESOURCE_FILES}
    )
else()
    add_executable(QFluentExample
        WIN32 MACOSX_BUNDLE
        ${CPP_SOURCES}
        ${HEADER_SOURCES}
        version.rc
        res/res.qrc
    )
endif()

target_link_libraries(QFluentExample PRIVATE QFluent)
target_link_libraries(QFluentExample
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Svg
        Qt${QT_VERSION_MAJOR}::Xml
)
if(WIN32 AND NOT BUILD_QWINDOWKIT)
    target_link_libraries(QFluentExample PRIVATE dwmapi)
endif()

if (BUILD_QWINDOWKIT)
    add_subdirectory(libs/qwindowkit)
    target_link_libraries(QFluentExample PRIVATE QWindowKit::Widgets)
    target_compile_definitions(QFluentExample PRIVATE USE_QWINDOWKIT=1)
else()
    target_compile_definitions(QFluentExample PRIVATE USE_QWINDOWKIT=0)
endif()

include(GNUInstallDirs)
install(TARGETS QFluentExample
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
