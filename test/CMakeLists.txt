cmake_minimum_required(VERSION 3.15)

set(BINARY ${CMAKE_PROJECT_NAME}_tst)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES true cpp_tests/*.h cpp_tests/*.cpp)

set(SOURCES ${TEST_SOURCES})

add_executable(${BINARY} ${TEST_SOURCES})

target_link_libraries(${BINARY} ${HDF5_CXX_LIBRARIES})
target_link_libraries(${BINARY} ${OpenCV_LIBRARIES})
target_link_libraries(${BINARY} ${Python3_LIBRARIES})
target_link_libraries(${BINARY} ${CMAKE_PROJECT_NAME}_lib gtest)

add_test(NAME ${BINARY} COMMAND ${BINARY})

