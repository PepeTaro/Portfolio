cmake_minimum_required(VERSION 3.10)

project(tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Eigen3
find_package(Eigen3 REQUIRED NO_MODULE)

# OpenMP
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# Gtest
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

SET(GCC_COVERAGE_COMPILE_FLAGS "-O3 -fopenmp -g")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
SET(INCLUDE_DIR ../include ../usr/local/include)
SET(SRC_DIR ../src)

SET(LIB_SRC_FILES
  ${SRC_DIR}/conv.cpp
  ${SRC_DIR}/dense.cpp
  ${SRC_DIR}/pooling.cpp
  ${SRC_DIR}/network.cpp
  ${SRC_DIR}/losses.cpp
  ${SRC_DIR}/activations.cpp
  ${SRC_DIR}/utils.cpp
  ${SRC_DIR}/im2col.cpp
  ${SRC_DIR}/weight_init.cpp
  ${SRC_DIR}/optimizer.cpp
  ${SRC_DIR}/save_load_model.cpp
  )

add_library(convnet ${LIB_SRC_FILES})
target_include_directories(convnet PUBLIC ${INCLUDE_DIR})
target_link_libraries(convnet Eigen3::Eigen)

SET(TEST_SRC_FILES
  optimizer_test.cpp
  network_test.cpp
  conv_test.cpp
  pooling_test.cpp
  dense_test.cpp
  losses_test.cpp
  activations_test.cpp
  utils_for_test.cpp
  utils_test.cpp
  im2col_test.cpp
  )

add_executable(gtest_main gtest_main.cpp ${TEST_SRC_FILES})
target_include_directories(gtest_main PRIVATE ../include/)
target_link_libraries(gtest_main LINK_PUBLIC convnet gtest Eigen3::Eigen)

add_executable(utils_test gtest_main.cpp utils_test.cpp utils_for_test.cpp)
target_include_directories(utils_test PRIVATE ../include/)
target_link_libraries(utils_test LINK_PUBLIC convnet gtest Eigen3::Eigen)

add_executable(im2col_test gtest_main.cpp im2col_test.cpp utils_for_test.cpp)
target_include_directories(im2col_test PRIVATE ../include/)
target_link_libraries(im2col_test LINK_PUBLIC  convnet gtest Eigen3::Eigen)

add_executable(activations_test gtest_main.cpp activations_test.cpp utils_for_test.cpp)
target_include_directories(activations_test PRIVATE ../include/)
target_link_libraries(activations_test LINK_PUBLIC  convnet gtest Eigen3::Eigen)

add_executable(losses_test gtest_main.cpp losses_test.cpp utils_for_test.cpp)
target_include_directories(losses_test PRIVATE ../include/)
target_link_libraries(losses_test LINK_PUBLIC  convnet gtest Eigen3::Eigen)

add_executable(dense_test gtest_main.cpp dense_test.cpp utils_for_test.cpp)
target_include_directories(dense_test PRIVATE ../include/)
target_link_libraries(dense_test LINK_PUBLIC  convnet gtest Eigen3::Eigen)

add_executable(pooling_test gtest_main.cpp pooling_test.cpp utils_for_test.cpp)
target_include_directories(pooling_test PRIVATE ../include/)
target_link_libraries(pooling_test LINK_PUBLIC  convnet gtest Eigen3::Eigen)

add_executable(conv_test gtest_main.cpp conv_test.cpp utils_for_test.cpp)
target_include_directories(conv_test PRIVATE ../include/)
target_link_libraries(conv_test LINK_PUBLIC  convnet gtest Eigen3::Eigen)

add_executable(network_test gtest_main.cpp network_test.cpp utils_for_test.cpp)
target_include_directories(network_test PRIVATE ../include/)
target_link_libraries(network_test LINK_PUBLIC  convnet gtest Eigen3::Eigen)

add_executable(optimizer_test gtest_main.cpp optimizer_test.cpp utils_for_test.cpp)
target_include_directories(optimizer_test PRIVATE ../include/)
target_link_libraries(optimizer_test LINK_PUBLIC  convnet gtest Eigen3::Eigen)
