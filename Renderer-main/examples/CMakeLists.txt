cmake_minimum_required(VERSION 3.10)

project(examples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# コンパイラーフラグ
SET(GCC_COVERAGE_COMPILE_FLAGS "-O3 -fopenmp -g")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

SET(INCLUDE_DIR ../include)
SET(SRC_DIR ../src)
SET(SRC_FILES
  ${SRC_DIR}/matrix4.cpp
  ${SRC_DIR}/vector4.cpp
  ${SRC_DIR}/mesh.cpp
  ${SRC_DIR}/color.cpp
  ${SRC_DIR}/transform.cpp
  ${SRC_DIR}/material.cpp
  ${SRC_DIR}/clip.cpp
  ${SRC_DIR}/light.cpp
  ${SRC_DIR}/camera.cpp
  ${SRC_DIR}/vertex.cpp
  ${SRC_DIR}/zbuffer.cpp
  ${SRC_DIR}/buffer.cpp
  ${SRC_DIR}/shading.cpp
  ${SRC_DIR}/pipeline.cpp
  ${SRC_DIR}/strip.cpp
  ${SRC_DIR}/texture.cpp
  )

add_library(renderer ${SRC_FILES})
target_include_directories(renderer PUBLIC ${INCLUDE_DIR})

add_executable(gear gear.cpp x11.cpp utils.cpp polygon.cpp)
target_link_libraries(gear LINK_PUBLIC renderer X11 Xext GL)

add_executable(shooter shooter.cpp x11.cpp utils.cpp polygon.cpp)
target_link_libraries(shooter LINK_PUBLIC renderer X11 Xext GL)

add_executable(plane plane.cpp x11.cpp utils.cpp polygon.cpp texture_loader.cpp)
target_link_libraries(plane LINK_PUBLIC renderer X11 Xext GL)

add_executable(sun_earth sun_earth.cpp x11.cpp utils.cpp polygon.cpp texture_loader.cpp)
target_link_libraries(sun_earth LINK_PUBLIC renderer X11 Xext GL)

add_executable(triangle triangle.cpp x11.cpp utils.cpp)
target_link_libraries(triangle LINK_PUBLIC renderer X11 Xext GL)

add_executable(sierpinski sierpinski.cpp x11.cpp utils.cpp polygon.cpp)
target_link_libraries(sierpinski LINK_PUBLIC renderer X11 Xext GL)

add_executable(light light.cpp x11.cpp utils.cpp polygon.cpp)
target_link_libraries(light LINK_PUBLIC renderer X11 Xext GL)
