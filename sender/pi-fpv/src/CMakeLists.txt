#cmake_minimum_required(VERSION 3.10)

#project(fpv_sender)

include_directories(
    camera
    camera/cam_driver
    )

add_subdirectory(camera)
link_libraries(camera)
add_executable(stream_test stream_cap_test.cpp camera)