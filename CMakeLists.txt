cmake_minimum_required(VERSION 3.16)
project(wish C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Werror)

add_executable(wish
        src/main.c
        src/util.c
        src/parser.c)
