cmake_minimum_required(VERSION 3.19)
project(QFluentExample LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Svg Xml)

qt_standard_project_setup()

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

qt_add_executable(QFluentExample
    WIN32 MACOSX_BUNDLE
    ${CPP_SOURCES}
    ${HEADER_SOURCES}
    version.rc
)

file(GLOB_RECURSE AUTO_RESOURCE_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    res/*.png res/*.jpg res/*.jpeg res/*.gif res/*.svg res/*.ico res/*.ttf res/*.qss
)

qt_add_resources(
    ${PROJECT_NAME}
    "QFluentExample"
    PREFIX "/"
    FILES ${AUTO_RESOURCE_FILES}
)

target_link_libraries(QFluentExample PRIVATE QFluent)

target_link_libraries(QFluentExample
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Svg
        Qt::Xml
)

include(GNUInstallDirs)

install(TARGETS QFluentExample
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET QFluentExample
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
