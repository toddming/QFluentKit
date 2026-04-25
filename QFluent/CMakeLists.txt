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
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
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

# 资源处理：必须在 add_library 之前设置 AUTORCC，确保 Qt6 下资源初始化顺序正确
if(${QT_VERSION_MAJOR} EQUAL 6)
    set(CMAKE_AUTORCC ON)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(QFluent SHARED ${QFLUENT_CPP_SOURCES})

target_sources(QFluent PRIVATE ${QFLUENT_HEADER_SOURCES})

target_include_directories(QFluent PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_compile_definitions(QFluent PRIVATE QFLUENT_LIBRARY)

# Debug 版本使用 'd' 后缀
set_target_properties(QFluent PROPERTIES
    DEBUG_POSTFIX "d"
)

# 资源文件挂载
if(${QT_VERSION_MAJOR} EQUAL 6)
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
option(QFLUENT_INSTALL_TO_QT "Install QFluent to Qt installation directory" ON)

if(QFLUENT_INSTALL_TO_QT)
    if(${QT_VERSION_MAJOR} EQUAL 6 AND DEFINED QT6_INSTALL_PREFIX)
        set(QT_INSTALL_DIR "${QT6_INSTALL_PREFIX}")
    elseif(${QT_VERSION_MAJOR} EQUAL 5 AND DEFINED _qt5Core_install_prefix)
        set(QT_INSTALL_DIR "${_qt5Core_install_prefix}")
    else()
        get_target_property(QT_CORE_IMPORTED_CONFIGS Qt${QT_VERSION_MAJOR}::Core IMPORTED_CONFIGURATIONS)
        list(GET QT_CORE_IMPORTED_CONFIGS 0 _first_cfg)
        string(TOUPPER "${_first_cfg}" _first_cfg_upper)
        get_target_property(QT_CORE_LOCATION Qt${QT_VERSION_MAJOR}::Core
            IMPORTED_LOCATION_${_first_cfg_upper}
        )
        get_filename_component(QT_LIB_DIR "${QT_CORE_LOCATION}" DIRECTORY)
        get_filename_component(QT_INSTALL_DIR "${QT_LIB_DIR}" DIRECTORY)
    endif()
    set(CMAKE_INSTALL_PREFIX "${QT_INSTALL_DIR}" CACHE PATH "Qt installation directory" FORCE)
    message(STATUS "QFluent will be installed to Qt directory: ${QT_INSTALL_DIR}")
endif()

# 安装核心头文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QFluent
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "QFluent" EXCLUDE
        PATTERN "Private" EXCLUDE
)

# 安装组件头文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/QFluent/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QFluent
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "Private" EXCLUDE
)

# ==================== 库文件安装 ====================
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

# ==================== MinGW 分离调试符号 ====================
if(MINGW)
    set_target_properties(QFluent PROPERTIES
        ENABLE_EXPORTS ON
    )

    find_program(CMAKE_OBJCOPY_FOUND objcopy)
    if(CMAKE_OBJCOPY_FOUND OR CMAKE_OBJCOPY)
        # 优先使用 CMake 自动检测到的 CMAKE_OBJCOPY，回退到 find_program 结果
        if(NOT CMAKE_OBJCOPY)
            set(CMAKE_OBJCOPY "${CMAKE_OBJCOPY_FOUND}")
        endif()

        add_custom_command(TARGET QFluent POST_BUILD
            # 仅在 Debug 配置下执行（生成器表达式在构建阶段求值，规避配置阶段的目标查找问题）
            COMMAND $<$<CONFIG:Debug>:${CMAKE_OBJCOPY}>
                    $<$<CONFIG:Debug>:--only-keep-debug>
                    $<$<CONFIG:Debug>:$<TARGET_FILE:QFluent>>
                    $<$<CONFIG:Debug>:$<TARGET_FILE:QFluent>.debug>
            COMMAND $<$<CONFIG:Debug>:${CMAKE_OBJCOPY}>
                    $<$<CONFIG:Debug>:--add-gnu-debuglink=$<TARGET_FILE:QFluent>.debug>
                    $<$<CONFIG:Debug>:$<TARGET_FILE:QFluent>>
            COMMENT "Generating separate debug symbols for MinGW (Debug only)"
        )

        if(POLICY CMP0087)
            cmake_policy(SET CMP0087 NEW)
        endif()

        install(CODE "
            if(EXISTS \"$<TARGET_FILE:QFluent>.debug\")
                file(INSTALL \"$<TARGET_FILE:QFluent>.debug\"
                    DESTINATION \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}\"
                )
            endif()
        " COMPONENT Debug)
    else()
        message(WARNING "objcopy not found, MinGW separate debug symbols will not be generated.")
    endif()
endif()

# ==================== CMake 配置文件安装 ====================
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/QFluentConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

set(CMAKE_INSTALL_INCLUDEDIR_FOR_CONFIG "${CMAKE_INSTALL_INCLUDEDIR}")
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/QFluentConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/QFluentConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QFluent
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR_FOR_CONFIG
)

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