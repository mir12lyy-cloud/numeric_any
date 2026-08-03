cmake_minimum_required(VERSION 4.0)
project(casyyy_numeric_any LANGUAGES CXX)

add_library(casyyy_numeric_any STATIC)
add_library(casyyy_numeric_any_for_hpp INTERFACE)

add_subdirectory(src)

target_sources(casyyy_numeric_any
    PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES numeric_any.ixx
)

target_sources(casyyy_numeric_any_for_hpp
        INTERFACE
        numeric_any.hpp
)

target_compile_features(casyyy_numeric_any PRIVATE cxx_std_20)
