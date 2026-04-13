cmake_minimum_required(VERSION 4.1)
project(ZombiAsm C)

set(CMAKE_C_STANDARD 23)

add_executable(${PROJECT_NAME}
        main.c
        lexer.c)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/hello.asm
        ${CMAKE_CURRENT_BINARY_DIR}/hello.asm
        COMMENT "Copying hello.asm to build directory"
)

option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()

    # Скачиваем Unity через FetchContent — не нужно ничего устанавливать вручную
    include(FetchContent)
    FetchContent_Declare(
            unity
            GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
            GIT_TAG        v2.6.0
    )
    FetchContent_MakeAvailable(unity)

    # Тестовый исполняемый файл
    add_executable(${PROJECT_NAME}_tests
            tests/test_lexer.c
            lexer.c             # тестируем тот же lexer.c
    )

    target_include_directories(${PROJECT_NAME}_tests PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/inc
    )

    target_link_libraries(${PROJECT_NAME}_tests PRIVATE unity)

    # Регистрируем в CTest — запускается через `ctest` или `cmake --build . --target test`
    add_test(NAME LexerTests COMMAND ${PROJECT_NAME}_tests)

    # Удобный таргет: make test_verbose
    add_custom_target(test_verbose
            COMMAND ${CMAKE_CTEST_COMMAND} --verbose
            DEPENDS ${PROJECT_NAME}_tests
    )
endif ()