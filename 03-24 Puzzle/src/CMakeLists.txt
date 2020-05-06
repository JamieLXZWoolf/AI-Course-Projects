cmake_minimum_required(VERSION 3.16)
project(t4puzzle)

set(CMAKE_CXX_STANDARD 17)

add_executable(t4puzzle main.cpp State.cpp State.h HeuristicSearch.cpp HeuristicSearch.h)