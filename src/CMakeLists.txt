cmake_minimum_required(VERSION 3.15)

set(BINARY ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true base/*.h base/*.cpp base/*.hpp base/eCP/*.h base/eCP/*.cpp base/eCP/*.hpp
      base/Metadata/*.h base/Metadata/*.cpp  base/r64/*.h base/r64/*.hpp base/r64/*.cpp *.h *.hpp *.cpp) 

set(SOURCES ${SOURCES})

add_executable(${BINARY}_run ${SOURCES})

if (WIN32)
      target_link_libraries(${BINARY}_run PRIVATE hdf5::hdf5-shared hdf5::hdf5_hl-shared)
      target_link_libraries(${BINARY}_run PRIVATE opencv_ml)
else()
      target_link_libraries(${BINARY}_run PRIVATE ${HDF5_CXX_LIBRARIES})
      target_link_libraries(${BINARY}_run PRIVATE ${OpenCV_LIBS})
endif()
target_link_libraries(${BINARY}_run PRIVATE ${Python3_LIBRARIES})

add_library(${BINARY}_lib STATIC ${SOURCES})
