cmake_minimum_required(VERSION 3.5)
project(QFluentExample LANGUAGES CXX)

# 支持Qt5和Qt6
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Svg Xml)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

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
add_executable(QFluentExample
    WIN32 MACOSX_BUNDLE
    ${CPP_SOURCES}
    ${HEADER_SOURCES}
    version.rc
)
file(GLOB_RECURSE AUTO_RESOURCE_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    res/*.png res/*.jpg res/*.jpeg res/*.gif res/*.svg res/*.ico res/*.ttf res/*.qss
)

# 资源处理：兼容Qt5和Qt6
if(${QT_VERSION_MAJOR} EQUAL 6)
    qt_add_resources(QFluentExample "QFluentExample"
        PREFIX "/"
        FILES ${AUTO_RESOURCE_FILES}
    )
else()
    # 对于Qt5，需要生成RCC源文件
    qt5_add_resources(RCC_SOURCES "QFluentExample" res/res.qrc)
    target_sources(QFluentExample PRIVATE ${RCC_SOURCES})
endif()

target_link_libraries(QFluentExample PRIVATE QFluent)
target_link_libraries(QFluentExample
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Svg
        Qt${QT_VERSION_MAJOR}::Xml
)
include(GNUInstallDirs)
install(TARGETS QFluentExample
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
