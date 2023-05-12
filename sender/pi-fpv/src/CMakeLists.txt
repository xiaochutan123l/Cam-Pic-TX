#cmake_minimum_required(VERSION 3.10)

#project(fpv_sender)

add_subdirectory(camera)

add_subdirectory(communication)

add_subdirectory(encoder)

add_executable(fpv_sender fpv_sender.cpp)

target_link_libraries(fpv_sender camera communication encoder)