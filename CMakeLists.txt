cmake_minimum_required(VERSION 3.10)
project(CPP_Labs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/eigen-3.4.0)

add_executable(CPP_Labs main.cpp
        src/utilities/timer.hpp
        src/utilities/utest.h
        src/grid.h
        src/asi.h)

target_link_libraries(CPP_Labs Eigen3::Eigen)
