cmake_minimum_required(VERSION 3.30)
project(numeric_any_example LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

function(compile_examples target_name source_name)
    add_executable(${target_name} ${source_name})
    target_sources(${target_name}
            PUBLIC
            FILE_SET CXX_MODULES
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../src
            FILES
            ../src/numeric_any_parser.ixx
            ../src/numeric_any.ixx
    )
    target_compile_features(${target_name} PUBLIC cxx_std_23)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /utf-8 /W4)
    endif()
endfunction()

compile_examples(heterogeneous_container heterogeneous_container.cpp)
compile_examples(construction construction.cpp)
compile_examples(metadata metadata.cpp)
compile_examples(type_lookup type_lookup.cpp)
compile_examples(operations operations.cpp)
compile_examples(bytes_view bytes_view.cpp)
compile_examples(casting casting.cpp)
compile_examples("output" "output.cpp")
compile_examples(visit visit.cpp)