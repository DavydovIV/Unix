cmake_minimum_required(VERSION 3.3)
project(chatik)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Chatik.cpp Chatik.h)
add_executable(chatik ${SOURCE_FILES})