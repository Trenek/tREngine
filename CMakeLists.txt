cmake_minimum_required(VERSION 3.10)

project(tREngine VERSION 1.0 LANGUAGES C)

add_subdirectory(dependencies)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(source)

add_library(tREngine ${ENGINE_FILES})
target_include_directories(tREngine PUBLIC
    ${ENGINE_DIRECTORIES}
    ${INCLUDE_DIRECTORIES}
)
target_link_libraries(tREngine PUBLIC ${LINK_LIBRARIES})

set(COMPILE_OPTIONS
    -W
    -g
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wunused
    -Wno-logical-op-parentheses
    -Wno-parentheses
    -Werror
    -Wno-comment
    -pedantic
    -pedantic-errors
    -D_GLIBCXX_DEBUG
    -D_GLIBCXX_DEBUG_PEDANTIC
    -D_UCRT
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(COMPILE_OPTIONS 
        ${COMPILE_OPTIONS}
        -O3
    )
    if (WIN32)
        target_link_options(tREngine PRIVATE -mwindows)
    endif()
else()
    set(COMPILE_OPTIONS 
        ${COMPILE_OPTIONS}
        -g
    )
endif()

target_compile_options(tREngine PUBLIC ${COMPILE_OPTIONS})
