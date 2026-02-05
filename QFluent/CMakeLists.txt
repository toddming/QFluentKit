cmake_minimum_required(VERSION 3.5)
project(QFluent LANGUAGES CXX)

# 支持Qt5和Qt6
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Svg Xml)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# MinGW 特殊配置
if(MINGW)
    set(CMAKE_CXX_FLAGS_DEBUG "-g1 -O0")
    set(CMAKE_C_FLAGS_DEBUG "-g1 -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-Os -s -DNDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "-Os -s -DNDEBUG")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -s")
    add_compile_options(-ffunction-sections -fdata-sections)
endif()

# 收集源文件与头文件（更明确、IDE 可见且对新增文件即时生效）
file(GLOB_RECURSE QFLUENT_CPP_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
)
file(GLOB_RECURSE QFLUENT_HEADER_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hh
)

if(NOT QFLUENT_CPP_SOURCES)
    message(FATAL_ERROR "No C/C++ sources found in ${CMAKE_CURRENT_SOURCE_DIR}/src.\nPlease add sources or adjust the glob patterns.")
endif()

add_library(QFluent SHARED ${QFLUENT_CPP_SOURCES})
target_sources(QFluent PRIVATE ${QFLUENT_HEADER_SOURCES})
target_include_directories(QFluent PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_compile_definitions(QFluent PRIVATE QFLUENT_LIBRARY)
# 资源处理：Qt6 使用 CMake AUTORCC，Qt5 使用 qt5_add_resources
if(${QT_VERSION_MAJOR} EQUAL 6)
    set(CMAKE_AUTORCC ON)
    target_sources(QFluent PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/res/res.qrc)
else()
    qt5_add_resources(RCC_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/res/res.qrc)
    target_sources(QFluent PRIVATE ${RCC_SOURCES})
endif()

target_link_libraries(QFluent
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Svg
        Qt${QT_VERSION_MAJOR}::Xml
)

include(GNUInstallDirs)
install(TARGETS QFluent
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
