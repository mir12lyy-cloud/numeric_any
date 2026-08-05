cmake_minimum_required(VERSION 4.0)
project(numeric_any_tests LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_numeric_any
    test_numeric_any.cpp
)

target_include_directories(test_numeric_any PRIVATE
    ${CMAKE_SOURCE_DIR}/../../src
)

if(CMAKE_CXX_STANDARD EQUAL 23)
    target_compile_features(test_numeric_any PUBLIC cxx_std_23)
else()
    target_compile_features(test_numeric_any PUBLIC cxx_std_20)
endif()

target_link_libraries(test_numeric_any PRIVATE
    GTest::gtest_main
)

enable_testing()
add_test(NAME NumericAnyTests COMMAND test_numeric_any)

if(MSVC)
    target_compile_options(test_numeric_any PRIVATE /W4 /utf-8)
endif()
