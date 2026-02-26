cmake_minimum_required(VERSION 3.19)
project(joy_core C)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(JOY_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../joy")
set(JOY_GENERATED_DIR "${CMAKE_BINARY_DIR}/joy_generated")
file(MAKE_DIRECTORY "${JOY_GENERATED_DIR}")

# Code generation: builtin table
file(GLOB JOY_PRIMITIVE_SOURCES CONFIGURE_DEPENDS
    "${JOY_SOURCE_DIR}/src/builtin/*.c"
    "${JOY_SOURCE_DIR}/src/builtin/*.h")

add_custom_command(
    OUTPUT "${JOY_GENERATED_DIR}/builtin.c" "${JOY_GENERATED_DIR}/builtin.h"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${JOY_GENERATED_DIR}"
    COMMAND ${Python3_EXECUTABLE} "${JOY_SOURCE_DIR}/tools/gen_builtins.py"
            "${JOY_SOURCE_DIR}" "${JOY_GENERATED_DIR}"
    DEPENDS ${JOY_PRIMITIVE_SOURCES} "${JOY_SOURCE_DIR}/tools/gen_builtins.py"
    VERBATIM
)

# Code generation: operation table
add_custom_command(
    OUTPUT "${JOY_GENERATED_DIR}/table.c"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${JOY_GENERATED_DIR}"
    COMMAND ${Python3_EXECUTABLE} "${JOY_SOURCE_DIR}/tools/gen_table.py"
            "${JOY_SOURCE_DIR}" "${JOY_GENERATED_DIR}/table.c"
    DEPENDS ${JOY_PRIMITIVE_SOURCES} "${JOY_SOURCE_DIR}/tools/gen_table.py"
    VERBATIM
)

add_custom_target(joy_generated
    DEPENDS
        "${JOY_GENERATED_DIR}/builtin.c"
        "${JOY_GENERATED_DIR}/builtin.h"
        "${JOY_GENERATED_DIR}/table.c"
)

# Joy core sources
set(JOY_CORE_SOURCES
    ${JOY_SOURCE_DIR}/src/error.c
    ${JOY_SOURCE_DIR}/src/factor.c
    ${JOY_SOURCE_DIR}/src/gc.c
    ${JOY_SOURCE_DIR}/src/interp.c
    ${JOY_SOURCE_DIR}/src/iolib.c
    ${JOY_SOURCE_DIR}/src/joy.c
    ${JOY_SOURCE_DIR}/src/module.c
    ${JOY_SOURCE_DIR}/src/optable.c
    ${JOY_SOURCE_DIR}/src/print.c
    ${JOY_SOURCE_DIR}/src/repl.c
    ${JOY_SOURCE_DIR}/src/scan.c
    ${JOY_SOURCE_DIR}/src/setraw.c
    ${JOY_SOURCE_DIR}/src/symbol.c
    ${JOY_SOURCE_DIR}/src/undefs.c
    ${JOY_SOURCE_DIR}/src/utils.c
    ${JOY_SOURCE_DIR}/src/write.c
)

add_library(joycore STATIC ${JOY_CORE_SOURCES})
add_dependencies(joycore joy_generated)

target_include_directories(joycore
    PUBLIC "${JOY_SOURCE_DIR}/include"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    PUBLIC "${JOY_GENERATED_DIR}" "${JOY_SOURCE_DIR}" "${JOY_SOURCE_DIR}/src/builtin"
)

target_compile_features(joycore PUBLIC c_std_11)

target_compile_definitions(joycore
    PUBLIC NOBDW JOY_NATIVE_TYPES
    PUBLIC VERS="NOBDW embedded"
    PUBLIC LINK=""
    PUBLIC COMP=""
)

if(NOT MSVC)
    target_compile_options(joycore PRIVATE
        -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-unused-result
        -Wno-error  # Don't treat warnings as errors in embedded context
        -fopenmp-simd
    )
    target_link_libraries(joycore PUBLIC m)
endif()
