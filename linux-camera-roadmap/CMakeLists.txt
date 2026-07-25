cmake_minimum_required(VERSION 3.16)

project(HelloOpenCV LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(heap_overflow_demo
    Day1Day2_memory/code/day2_heap_overflow.cpp
)

target_include_directories(heap_overflow_demo PRIVATE
    Day1Day2_memory/include
)

target_compile_options(heap_overflow_demo PRIVATE
    -fsanitize=address
    -fno-omit-frame-pointer
    -g
    -O0
)

target_link_options(heap_overflow_demo PRIVATE
    -fsanitize=address
)


add_executable(memory_leak_demo
    Day1Day2_memory/code/day2_memory_leak.cpp
)

target_include_directories(memory_leak_demo PRIVATE
    Day1Day2_memory/include
)

target_compile_options(memory_leak_demo PRIVATE
    -g
    -O0
)