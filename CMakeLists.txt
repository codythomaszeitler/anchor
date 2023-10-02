cmake_minimum_required(VERSION 3.25.1)

project(anchor VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("${PROJECT_SOURCE_DIR}"/src)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(main ${PROJECT_SOURCE_DIR}/src/main.cc ${PROJECT_SOURCE_DIR}/src/lexer.cc ${PROJECT_SOURCE_DIR}/src/llvm.cc)

enable_testing()

add_executable(
    main_test
    ${PROJECT_SOURCE_DIR}/src/lexer.cc
    ${PROJECT_SOURCE_DIR}/src/parser.cc
    ${PROJECT_SOURCE_DIR}/src/llvm.cc
    ${PROJECT_SOURCE_DIR}/test/main_test.cc
    ${PROJECT_SOURCE_DIR}/test/lexer_test.cc
    ${PROJECT_SOURCE_DIR}/test/parser_test.cc
    ${PROJECT_SOURCE_DIR}/test/llvm_test.cc
)

target_link_libraries(
    main_test
    GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(main_test)
