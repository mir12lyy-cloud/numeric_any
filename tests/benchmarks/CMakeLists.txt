cmake_minimum_required(VERSION 3.30)
project(numeric_any_benchmarks LANGUAGES CXX)

set(CMAKE_CXX_SCAN_FOR_MODULES ON)

include(FetchContent)
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.9.1
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wno-c2y-extensions -Wno-error=c2y-extensions)
endif()
FetchContent_MakeAvailable(googlebenchmark)

set(NUMERIC_ANY_INC ${CMAKE_SOURCE_DIR}/../../src)

function(setup_bench_target target_name source_name)
    add_executable(${target_name} ${source_name})
    target_include_directories(${target_name} PRIVATE ${NUMERIC_ANY_INC})
    if(CMAKE_CXX_STANDARD EQUAL 23)
        target_compile_features(${target_name} PUBLIC cxx_std_23)
    else()
        target_compile_features(${target_name} PUBLIC cxx_std_20)
    endif()
    target_link_libraries(${target_name} PRIVATE benchmark::benchmark)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /utf-8 /W4)
    endif()
endfunction()

function(setup_bench_target_ixx target_name source_name)
    add_executable(${target_name} ${source_name})
    target_sources(${target_name}
        PUBLIC
            FILE_SET CXX_MODULES
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../src
            FILES
                ../../src/numeric_any_parser.ixx
                ../../src/numeric_any.ixx
    )
    if(CMAKE_CXX_STANDARD EQUAL 23)
        target_compile_features(${target_name} PUBLIC cxx_std_23)
    else()
        target_compile_features(${target_name} PUBLIC cxx_std_20)
    endif()
    target_link_libraries(${target_name} PRIVATE benchmark::benchmark)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /utf-8 /W4)
    endif()
endfunction()

setup_bench_target(bench_same_type    same_type_bench.cpp)
setup_bench_target(bench_mixed_type   mixed_type_bench.cpp)
setup_bench_target(bench_conversion   conversion_bench.cpp)
setup_bench_target_ixx(bench_same_type_ixx    same_type_bench_ixx.cpp)
setup_bench_target_ixx(bench_mixed_type_ixx   mixed_type_bench_ixx.cpp)
setup_bench_target_ixx(bench_conversion_ixx   conversion_bench_ixx.cpp)