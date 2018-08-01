cmake_minimum_required(VERSION 3.10)
project(crct C)

set(CMAKE_C_STANDARD 90)

add_executable(crct src/main.c "src/data structures/linkedList.c" "src/data structures/linkedList.h" "src/data structures/stack.c" "src/data structures/stack.h" "src/data structures/tree.c" "src/data structures/tree.h")