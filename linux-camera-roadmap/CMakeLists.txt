cmake_minimum_required(VERSION 3.16)

project(HelloOpenCV LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(hello_opencv
    code/main.cpp
)

target_link_libraries(hello_opencv
    PRIVATE
    ${OpenCV_LIBS}
)

target_include_directories(hello_opencv
    PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)