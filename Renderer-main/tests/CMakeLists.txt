cmake_minimum_required(VERSION 3.10)

project(tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Gtest
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

SET(INCLUDE_DIR ../include)
SET(SRC_DIR ../src)

add_executable(test_vector4 test_vector4.cpp ${SRC_DIR}/vector4.cpp)
target_include_directories(test_vector4 PRIVATE ${INCLUDE_DIR})
target_link_libraries(test_vector4 LINK_PUBLIC gtest pthread)

add_executable(test_matrix4 test_matrix4.cpp ${SRC_DIR}/matrix4.cpp ${SRC_DIR}/vector4.cpp)
target_include_directories(test_matrix4 PRIVATE ${INCLUDE_DIR})
target_link_libraries(test_matrix4 LINK_PUBLIC gtest pthread)
