cmake_policy(SET CMP0048 NEW)
cmake_minimum_required(VERSION 3.27)
project(Exquisitor VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20 /O2")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3")
endif(WIN32)

# Python3
find_package (Python3 COMPONENTS Interpreter Development REQUIRED)
link_directories(${Python3_LIBRARY_DIRS})
include_directories(${Python3_INCLUDE_DIRS})

# HDF5
find_package(HDF5 CONFIG REQUIRED)
include_directories(${HDF5_INCLUDE_DIRS})

#OpenCV
find_package(OpenCV CONFIG REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Project Files
include_directories(src)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib/googletest)

