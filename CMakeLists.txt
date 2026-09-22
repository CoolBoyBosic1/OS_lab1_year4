cmake_minimum_required(VERSION 3.16)
project(wish CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Werror)

add_executable(wish
        src/main.cpp
        src/error.cpp
        src/parser.cpp
        src/path.cpp
        src/builtins.cpp
        src/executor.cpp)

# Офіційний тестер шукає виконуваний файл ./wish поряд із каталогом tests/,
# тому кладемо бінарник у корінь проєкту, а не в cmake-build-*/.
set_target_properties(wish PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Ціль "run_tests" видно у списку конфігурацій CLion:
# вона збирає wish і запускає всі 22 офіційні тести.
add_custom_target(run_tests
        COMMAND ${CMAKE_SOURCE_DIR}/test-wish.sh -c
        DEPENDS wish
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        USES_TERMINAL)
