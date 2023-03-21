cmake_minimum_required(VERSION 3.15)

set(BINARY ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true base/r64/*.h base/r64/*.hpp base/r64/*.cpp base/*.h base/*.cpp base/*.hpp base/eCP/*.h base/eCP/*.cpp base/eCP/*.hpp
      base/Metadata/*.h base/Metadata/*.cpp *.h *.hpp *.cpp)

set(SOURCES ${SOURCES})

add_library(${BINARY}_lib STATIC ${SOURCES})

add_executable(${BINARY}_run ${SOURCES})

target_link_libraries(${BINARY}_run ${HDF5_CXX_LIBRARIES})
target_link_libraries(${BINARY}_run ${OpenCV_LIBS})
target_link_libraries(${BINARY}_run ${Python3_LIBRARIES})

