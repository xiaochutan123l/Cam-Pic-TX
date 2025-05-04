set(SRC_LIST 
    cam_controller_impl.cpp
    fmt_utils.cpp
    )

add_subdirectory(cam_driver)

#add_library(camera ${SRC_LIST})

if (NOT TARGET camera)
    add_library(camera ${SRC_LIST})
endif()

target_include_directories(camera PUBLIC cam_driver)
target_include_directories(camera PUBLIC ../../include/camera)

target_link_libraries(camera cam_driver_lib)