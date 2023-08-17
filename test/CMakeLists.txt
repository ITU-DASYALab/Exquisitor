cmake_minimum_required(VERSION 3.15)

set(BINARY ${CMAKE_PROJECT_NAME}_tst)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES true cpp_tests/*.h cpp_tests/*.cpp)

set(SOURCES ${TEST_SOURCES})

add_executable(${BINARY} ${TEST_SOURCES})

if (WIN32)
      target_link_libraries(${BINARY} PRIVATE hdf5::hdf5-shared hdf5::hdf5_hl-shared)
      target_link_libraries(${BINARY} PRIVATE opencv_ml)
else()
      target_link_libraries(${BINARY} PRIVATE ${HDF5_CXX_LIBRARIES})
      target_link_libraries(${BINARY} PRIVATE ${OpenCV_LIBS})
endif()
target_link_libraries(${BINARY} PRIVATE ${Python3_LIBRARIES})
target_link_libraries(${BINARY} PRIVATE ${CMAKE_PROJECT_NAME}_lib gtest)

add_test(NAME ${BINARY} COMMAND ${BINARY})

