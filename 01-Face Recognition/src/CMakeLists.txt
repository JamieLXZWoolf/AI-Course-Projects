cmake_minimum_required(VERSION 3.15)
project(ffnnFaceRecognition)

set(CMAKE_CXX_STANDARD 17)

add_executable(face main.cpp pgmImage.h ffnn.h pgmImage.cpp ffnn.cpp)