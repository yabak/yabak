cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(yabak
        LANGUAGES CXX
        VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp)

add_executable(yabak ${SOURCE_FILES})
