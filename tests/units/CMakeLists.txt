cmake_minimum_required(VERSION 3.30)
project(numeric_any_tests LANGUAGES CXX)

set(CMAKE_CXX_SCAN_FOR_MODULES ON)

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
add_executable(test_numeric_any_ixx
    test_numeric_any_ixx.cpp
)

target_sources(test_numeric_any_ixx
    PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../src
        FILES 
            ../../src/numeric_any.ixx
)

target_include_directories(test_numeric_any PRIVATE
    ${CMAKE_SOURCE_DIR}/../../src
)

if(CMAKE_CXX_STANDARD EQUAL 23)
    target_compile_features(test_numeric_any PUBLIC cxx_std_23)
    target_compile_features(test_numeric_any_ixx PUBLIC cxx_std_23)
else()
    target_compile_features(test_numeric_any PUBLIC cxx_std_20)
    target_compile_features(test_numeric_any_ixx PUBLIC cxx_std_20)
endif()

target_link_libraries(test_numeric_any PRIVATE
    GTest::gtest_main
)

target_link_libraries(test_numeric_any_ixx PRIVATE
    GTest::gtest_main
)

enable_testing()
add_test(NAME NumericAnyTests COMMAND test_numeric_any)
add_test(NAME NumericAnyTestsIxx COMMAND test_numeric_any_ixx)

if(MSVC)
    target_compile_options(test_numeric_any PRIVATE /W4 /utf-8)
    target_compile_options(test_numeric_any_ixx PRIVATE /W4 /utf-8)
endif()
