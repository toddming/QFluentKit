cmake_minimum_required(VERSION 3.16)

project(QFluentKit VERSION 0.1 LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(QFLUENT_IS_TOP_LEVEL ON)
else()
    set(QFLUENT_IS_TOP_LEVEL OFF)
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_subdirectory(QFluent)

# 示例程序只在把本工程当作顶层项目时才构建；
# 作为子目录被其他工程引用时不应引入示例目标。
if(QFLUENT_IS_TOP_LEVEL)
    add_subdirectory(QFluentExample)
endif()

option(QFLUENT_BUILD_TESTS "Build QFluent unit tests" ${QFLUENT_IS_TOP_LEVEL})
if(QFLUENT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# find_package(QFluent QUIET)
# if(QFluent_FOUND)
#     add_subdirectory(MinimalExample)
# endif()


