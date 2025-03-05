cmake_minimum_required(VERSION 3.30)
project(BMPViewerCLI)

set(CMAKE_CXX_STANDARD 20)

add_executable(BMPViewerCLI main.cpp
        BitMapHeader.h)
