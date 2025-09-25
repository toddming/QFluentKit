cmake_minimum_required(VERSION 3.19)
project(QFluent LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Svg Xml)

qt_standard_project_setup()

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

qt_add_resources(
    ${PROJECT_NAME}
    "QFluent"
    PREFIX "/"
    FILES ${AUTO_RESOURCE_FILES}
)

# 把生成的资源源文件加入库
target_sources(QFluent PRIVATE ${AUTO_RESOURCE_FILES})


target_link_libraries(QFluent
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Svg
        Qt::Xml
)

add_subdirectory(libs/qwindowkit)

target_link_libraries(QFluent PRIVATE QWindowKit::Widgets)


include(GNUInstallDirs)

install(TARGETS QFluent
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET QFluent
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
