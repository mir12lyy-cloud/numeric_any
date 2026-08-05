cmake_minimum_required(VERSION 3.30)
project(numeric_any LANGUAGES CXX)

add_library(numeric_any STATIC)
add_library(numeric_any_for_hpp INTERFACE)

target_sources(numeric_any
    PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
        FILES 
        src/numeric_any.ixx
)

target_include_directories(numeric_any_for_hpp
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

if(CMAKE_CXX_STANDARD EQUAL 23)
    target_compile_features(numeric_any PUBLIC cxx_std_23)
else()
    target_compile_features(numeric_any PUBLIC cxx_std_20)
endif()
