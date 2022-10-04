cmake_policy(SET CMP0048 NEW)
project(Exquisitor VERSION 1.0)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3")

# Python3
find_package (Python3 COMPONENTS Interpreter Development)
link_directories(${Python3_LIBRARY_DIRS})
include_directories(${Python3_INCLUDE_DIRS})

# HDF5
find_package(HDF5 COMPONENTS CXX REQUIRED)
link_directories(${HDF5_LIBRARY_DIRS})
include_directories(${HDF5_INCLUDE_DIRS})

# OpenCV
find_package(OpenCV 4.5.5 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBS})

# Project Files
include_directories(src)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib/googletest)

