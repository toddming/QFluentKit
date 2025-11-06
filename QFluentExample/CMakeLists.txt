cmake_minimum_required(VERSION 3.19)
project(QFluentExample LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Svg Xml)

qt_standard_project_setup()

option(EXTERNAL_QFLUENT "Enable QFluentKit support" ON)

if(EXTERNAL_QFLUENT)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/src)
endif()

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

if(EXTERNAL_QFLUENT)
    set(DLL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/bin/$<CONFIG>")
    add_custom_command(TARGET QFluentExample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${DLL_SOURCE_DIR}
            $<TARGET_FILE_DIR:QFluentExample>
        COMMENT "Copying DLLs from ${DLL_SOURCE_DIR}"
    )

    target_link_libraries(QFluentExample PRIVATE
        $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/Debug/QFluent.lib>
        $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/Release/QFluent.lib>
    )
else()
    target_link_libraries(QFluentExample PRIVATE QFluent)
endif()


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
