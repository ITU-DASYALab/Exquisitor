cmake_minimum_required(VERSION 3.15)
project(Exquisitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")

# HDF5
find_package(HDF5 COMPONENTS CXX REQUIRED)
link_directories(${HDF5_LIBRARY_DIRS})
include_directories(${HDF5_INCLUDE_DIRS})

# HDFql (Mac OS)
#set(HDFQL_ROOT ${PROJECT_SOURCE_DIR}/external/hdfql-2.1.0)
#include_directories(${HDFQL_ROOT}/include)

# Project Files
include_directories(src)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib/googletest)

