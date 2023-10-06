cmake_minimum_required(VERSION 3.26)
project(clox C)

include(cmake/compiler_options.cmake)

set(CMAKE_C_STANDARD 11)

set(CLOX_SOURCES
    src/chunk.c
    src/debug.c
    src/main.c
    src/memory.c
    src/value.c
)

set(CLOX_HEADERS
    include/chunk.h
    include/debug.h
    include/memory.h
    include/value.h
)

add_executable(clox ${CLOX_SOURCES} ${CLOX_HEADERS})

target_include_directories(clox PRIVATE include)
set_compiler_flags(clox)
