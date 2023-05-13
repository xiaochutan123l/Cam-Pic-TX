add_executable(camera_test stream_cap_test.cpp)

#add_executable(v4l2_test v4l2_framerate_test.c)

target_include_directories(camera_test PUBLIC ../../include/camera)
target_link_libraries(camera_test PUBLIC camera)
#target_include_directories(v4l2_test PUBLIC ../../src/camera/cam_driver)
