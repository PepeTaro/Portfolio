cmake_minimum_required(VERSION 3.10)

project(examples)

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

SET(GCC_COVERAGE_COMPILE_FLAGS "-Ofast -fopenmp")
#SET(GCC_COVERAGE_COMPILE_FLAGS "-O3 -fopenmp -g")

SET(INCLUDE_DIR ../include)
SET(SRC_DIR ../src)
SET(LIB_SRC_FILES ${SRC_DIR}/conv.cpp ${SRC_DIR}/dense.cpp ${SRC_DIR}/pooling.cpp ${SRC_DIR}/network.cpp ${SRC_DIR}/losses.cpp ${SRC_DIR}/activations.cpp ${SRC_DIR}/utils.cpp ${SRC_DIR}/im2col.cpp ${SRC_DIR}/weight_init.cpp ${SRC_DIR}/optimizer.cpp ${SRC_DIR}/save_load_model.cpp)
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

add_library(convnet ${LIB_SRC_FILES})
target_include_directories(convnet PUBLIC ${INCLUDE_DIR})
target_link_libraries(convnet Eigen3::Eigen)

add_executable(mnist_demo mnist_demo.cpp)
target_link_libraries(mnist_demo LINK_PUBLIC convnet Eigen3::Eigen sfml-graphics sfml-window sfml-system)

add_executable(cifar10_demo cifar10_demo.cpp)
target_link_libraries(cifar10_demo LINK_PUBLIC convnet Eigen3::Eigen)

add_executable(mnist_drawing mnist_drawing.cpp)
target_link_libraries(mnist_drawing LINK_PUBLIC convnet Eigen3::Eigen sfml-graphics sfml-window sfml-system)

add_executable(mnist_test mnist_test.cpp)
target_link_libraries(mnist_test LINK_PUBLIC convnet Eigen3::Eigen sfml-graphics sfml-window sfml-system)

add_executable(cifar10_test cifar10_test.cpp)
target_link_libraries(cifar10_test LINK_PUBLIC convnet Eigen3::Eigen sfml-graphics sfml-window sfml-system)
