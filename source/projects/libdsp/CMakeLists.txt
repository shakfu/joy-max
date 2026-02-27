cmake_minimum_required(VERSION 3.19)
project(libdsp C)

add_library(dsp STATIC
    dsp_graph.c
    dsp_graph.h
)

target_include_directories(dsp
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_features(dsp PUBLIC c_std_11)

if(NOT MSVC)
    target_compile_options(dsp PRIVATE
        -Wall -Wextra -Wpedantic -Wno-unused-parameter
    )
    target_link_libraries(dsp PUBLIC m)
endif()

# Test executable
add_executable(test_dsp_graph test_dsp_graph.c)
target_link_libraries(test_dsp_graph PRIVATE dsp)
if(NOT MSVC)
    target_compile_options(test_dsp_graph PRIVATE
        -Wall -Wextra -Wpedantic -Wno-unused-parameter
    )
endif()

enable_testing()
add_test(NAME dsp_graph_tests COMMAND test_dsp_graph)
