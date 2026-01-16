cmake_minimum_required(VERSION 3.5)
project(QFluent LANGUAGES CXX)

# 支持Qt5和Qt6
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Svg Xml)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ============================================
# 编译优化配置 - 减小文件大小
# ============================================

# MinGW 特殊配置
if(MINGW)
    # Debug 模式：使用最小调试信息（-g1）而不是完整调试信息（-g3）
    set(CMAKE_CXX_FLAGS_DEBUG "-g1 -O0")
    set(CMAKE_C_FLAGS_DEBUG "-g1 -O0")

    # Release 模式：优化大小并移除符号
    set(CMAKE_CXX_FLAGS_RELEASE "-Os -s -DNDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "-Os -s -DNDEBUG")

    # 链接器优化
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -s")

    # 函数和数据段分离，便于链接器移除未使用代码
    add_compile_options(-ffunction-sections -fdata-sections)

    # 可选：分离调试信息到单独文件（Debug模式）
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        # add_compile_options(-gsplit-dwarf)  # 需要 dwarf-4 支持，可选启用
    endif()
endif()

# MSVC 配置
if(MSVC)
    # Release 模式优化大小
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O1 /Oi /Ot /GL")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG /OPT:REF /OPT:ICF")

    # 移除不必要的运行时检查
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
endif()

# 通用优化
# if(CMAKE_BUILD_TYPE STREQUAL "Release")
#     # 启用链接时优化（LTO）- 可以显著减小文件大小
#     set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

#     # 移除运行时类型信息（如果不需要的话）
#     # add_compile_options(-fno-rtti)  # 取消注释以禁用 RTTI
# endif()

# ============================================
# 源文件配置
# ============================================

# 递归查找 src 目录下所有 .cpp 和 .h 文件
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

add_library(QFluent SHARED ${CPP_SOURCES})
target_sources(QFluent PRIVATE ${HEADER_SOURCES})

target_include_directories(QFluent PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_compile_definitions(QFluent PRIVATE QFLUENT_LIBRARY)

# ============================================
# 资源文件配置
# ============================================

file(GLOB_RECURSE AUTO_RESOURCE_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    res/*.png res/*.jpg res/*.jpeg res/*.gif res/*.svg res/*.ico res/*.ttf res/*.qss
)

# 资源处理：兼容Qt5和Qt6
if(${QT_VERSION_MAJOR} EQUAL 6)
    qt_add_resources(QFluent "QFluent"
        PREFIX "/"
        FILES ${AUTO_RESOURCE_FILES}
    )
else()
    # 对于Qt5，需要生成RCC源文件
    qt5_add_resources(RCC_SOURCES res/res.qrc)
    target_sources(QFluent PRIVATE ${RCC_SOURCES})
endif()

# ============================================
# 链接库配置
# ============================================

target_link_libraries(QFluent
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Svg
        Qt${QT_VERSION_MAJOR}::Xml
)

add_subdirectory(libs/qwindowkit)
target_link_libraries(QFluent PRIVATE QWindowKit::Widgets)

# ============================================
# 安装配置
# ============================================

include(GNUInstallDirs)
install(TARGETS QFluent
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# ============================================
# 后处理：自动剥离符号（可选）
# ============================================

if(MINGW AND CMAKE_BUILD_TYPE STREQUAL "Release")
    # 在安装后自动使用 strip 剥离符号
    install(CODE "
        execute_process(
            COMMAND ${CMAKE_STRIP} --strip-all \${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/libQFluent.dll
            ERROR_QUIET
        )
    ")
endif()
