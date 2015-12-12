cmake_minimum_required(VERSION 3.3)
project(vebchik)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp VebChik.cpp VebChik.h)
add_executable(vebchik ${SOURCE_FILES})