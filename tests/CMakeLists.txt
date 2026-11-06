cmake_minimum_required(VERSION 3.16)

# QT_VERSION_MAJOR 在 QFluent 子目录中设置，不会泄漏到同级目录，这里重新探测一次
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Widgets)

set(QFLUENT_TEST_TARGETS "")

function(qfluent_add_test test_name)
    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name} PRIVATE
        QFluent
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets
    )

    # 让测试可执行文件旁边就有 QFluent.dll / libQFluent.so，省去手工配置 PATH
    add_custom_command(TARGET ${test_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:QFluent> $<TARGET_FILE_DIR:${test_name}>
    )

    if(WIN32)
        # GetProcessMemoryInfo 用于内存回归检查
        target_link_libraries(${test_name} PRIVATE psapi)
    endif()

    add_test(NAME ${test_name} COMMAND ${test_name})

    # 无显示环境下也能跑：offscreen 平台插件不需要真实窗口系统
    set_tests_properties(${test_name} PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
        TIMEOUT 300
    )
endfunction()

qfluent_add_test(tst_theme)
qfluent_add_test(tst_stylesheet)
qfluent_add_test(tst_widgets)
