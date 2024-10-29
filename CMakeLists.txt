cmake_minimum_required(VERSION 3.10)
project(CPP_Labs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(CPP_Labs main.cpp
        src/utilities/mpl_writer.hpp
        src/utilities/point_reader.hpp
        src/utilities/random_points.hpp
        src/utilities/timer.hpp
        src/utilities/utest.h
        src/bucket_quadtree.h
        src/direct_search.h)
