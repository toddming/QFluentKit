cmake_minimum_required(VERSION 3.5)
project(QFluent LANGUAGES CXX)

# 支持Qt5和Qt6
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Svg Xml)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

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
endif()

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
