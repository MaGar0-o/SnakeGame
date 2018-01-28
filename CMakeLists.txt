cmake_minimum_required(VERSION 3.9)
project(snake C)

set(CMAKE_C_STANDARD 99)

add_executable(snake main.c)

include_directories(/usr/include/SDL2)
target_link_libraries(snake m SDL2 SDL2_gfx)
add_definitions(-D_REENTRANT)
