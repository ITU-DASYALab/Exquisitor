set(BINARY ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp *.hpp eCP/*.h eCP/*.cpp eCP/*.hpp)

set(SOURCES ${SOURCES})

add_executable(${BINARY}_run ${SOURCES})

target_link_libraries(${BINARY}_run ${HDF5_CXX_LIBRARIES})
target_link_libraries(${BINARY}_run ${OpenCV_LIBS})
target_link_libraries(${BINARY}_run ${Python3_LIBRARIES})

add_library(${BINARY}_lib STATIC ${SOURCES})