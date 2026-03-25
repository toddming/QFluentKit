cmake_minimum_required(VERSION 3.5)
project(QFluent VERSION 0.1.0 LANGUAGES CXX)

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

# Debug 版本使用 'd' 后缀（类似 Qt 的做法：Qt6Cored.dll vs Qt6Core.dll）
set_target_properties(QFluent PROPERTIES
    DEBUG_POSTFIX "d"
)
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

# ==================== 安装配置 ====================
# 安装选项：是否安装到 Qt 目录（默认关闭，使用 CMAKE_INSTALL_PREFIX）
option(QFLUENT_INSTALL_TO_QT "Install QFluent to Qt installation directory" OFF)

# 设置 Qt 安装路径
if(QFLUENT_INSTALL_TO_QT)
    # 获取 Qt 安装路径
    get_target_property(QT_CORE_LOCATION Qt${QT_VERSION_MAJOR}::Core LOCATION)
    get_filename_component(QT_LIB_DIR "${QT_CORE_LOCATION}" DIRECTORY)
    get_filename_component(QT_INSTALL_DIR "${QT_LIB_DIR}" DIRECTORY)
    set(CMAKE_INSTALL_PREFIX "${QT_INSTALL_DIR}" CACHE PATH "Qt installation directory" FORCE)
    message(STATUS "QFluent will be installed to Qt directory: ${QT_INSTALL_DIR}")
endif()

# 公开头文件目录（用于安装）
# 注意：src/ 根目录包含核心头文件，src/QFluent/ 包含组件头文件
# Private/ 目录不安装（PIMPL 私有实现）

# 安装核心头文件（src/ 根目录，排除 Private 和 QFluent 子目录）
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FluentGlobal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Theme.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FluentIcon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Router.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Animation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Screen.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StyleSheet.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TextWrap.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QFluent
)

# 安装组件头文件（src/QFluent/ 目录）
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/QFluent/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QFluent
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# 安装资源文件（图标、样式表等）
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/res/
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/QFluent/res
)

# ==================== 库文件安装 ====================
# Windows 平台：Debug 版本使用 'd' 后缀，与 Release 版本共存于同一目录
install(TARGETS QFluent
    EXPORT QFluentTargets
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# 安装 PDB 文件（MSVC Debug）
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:QFluent>
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        CONFIGURATIONS Debug
        OPTIONAL
    )
endif()

# ==================== CMake 配置文件安装 ====================
include(CMakePackageConfigHelpers)

# 生成版本配置文件
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/QFluentConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# 配置文件模板
set(CMAKE_INSTALL_INCLUDEDIR_FOR_CONFIG "${CMAKE_INSTALL_INCLUDEDIR}")
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/QFluentConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/QFluentConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QFluent
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR_FOR_CONFIG
)

# 安装 CMake 配置文件
install(EXPORT QFluentTargets
    FILE QFluentTargets.cmake
    NAMESPACE QFluent::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QFluent
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/QFluentConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/QFluentConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QFluent
)
