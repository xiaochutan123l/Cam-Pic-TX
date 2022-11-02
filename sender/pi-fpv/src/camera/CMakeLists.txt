set(SRC_LIST 
    cam_controller_impl.cpp
    fmt_utils.cpp

    cam_driver
    )

add_subdirectory(cam_driver)

#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

include_directories(cam_driver)

link_libraries(cam_driver)

add_library(camera ${SRC_LIST})