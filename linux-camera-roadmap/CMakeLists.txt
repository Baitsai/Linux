cmake_minimum_required(VERSION 3.16)

project(HelloOpenCV LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(main
    Day3_STL/code/day3_main.cpp
)

target_link_libraries(main
    PRIVATE
    ${OpenCV_LIBS}
)

target_include_directories(main
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Day3_STL/include
)

target_compile_options(main
    PRIVATE
    -fsanitize=address
    -fno-omit-frame-pointer
    -g
)

target_link_options(main
    PRIVATE
    -fsanitize=address
)
