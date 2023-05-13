set(SRC_LIST 
    cam_controller_impl.cpp
    fmt_utils.cpp
    )

add_subdirectory(cam_driver)

add_library(camera ${SRC_LIST})

target_include_directories(camera PUBLIC cam_driver)
target_include_directories(camera PUBLIC ../../include/camera)

target_link_libraries(camera cam_driver)

#target_compile_features(solarSystemSimulator PUBLIC cxx_std_17)