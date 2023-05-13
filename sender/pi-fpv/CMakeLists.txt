cmake_minimum_required(VERSION 3.18)

project(fpv_sender VERSION 1.0 LANGUAGES C CXX)

#add_subdirectory(src test)

#include_directories(include)

#set(CMAKE_CXX_FLAGS_RELEASE "-O2") 

add_subdirectory(src)

add_subdirectory(test)

#add_subdirectory(app)