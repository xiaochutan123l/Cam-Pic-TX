set(SRC_LIST 
    v4l2_capture.c;
    )

#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_library(cam_driver ${SRC_LIST})