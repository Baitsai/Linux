### Linux
## Day 1: Hello OpenCV 
# Cmake
1. 建立專案與設定 C++ 版本
    cmake_minimum_required(VERSION 3.16)
    project(MyProject LANGUAGES CXX)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

2. 建立執行檔
    add_executable(hello_opencv
        main.cpp
    )

3. 尋找第三方套件
    find_package(OpenCV REQUIRED)

4. 連結函式庫
    target_link_libraries(hello_opencv
        PRIVATE
        ${OpenCV_LIBS}
    )
    
5. 加入標頭檔路徑
    target_include_directories(hello_opencv
        PRIVATE
        include
    )

標準建置流程:
cmake -S . -B build
cmake --build build

CMakeLists.txt
    ↓ CMake 讀取
產生建置系統
    ↓
AppleClang / g++ 編譯與連結
    ↓
執行檔

建立第一個使用 CMake 建置的 OpenCV C++ 專案。

# 目前功能

1. 從命令列取得圖片路徑
2. 使用 OpenCV 讀取圖片
3. 顯示圖片尺寸與通道數
4. 儲存圖片副本
5. 使用視窗顯示圖片

# Project Structure
linux-camera-roadmap/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── cat.png
├── code/
│   └── main.cpp
└── build/

# Requirements
C++17
CMake
OpenCV
AppleClang、Clang 或 GCC

macOS 用 Homebrew 安裝：
    brew install cmake
    brew install opencv
    Build

在專案根目錄執行：
    cmake -S . -B build
    cmake --build build
    Run
    ./build/hello_opencv assets/test.jpg

圖片也可以使用其他路徑：
    ./build/hello_opencv /path/to/image.png
    Expected Output

終端機會顯示：
    Image width: ...
    Image height: ...
    Channels: ...
    顯示圖片視窗
