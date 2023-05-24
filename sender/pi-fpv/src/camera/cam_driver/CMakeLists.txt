set(CAMERA_DRIVER_TARGET cam_driver)

set(SRC_LIST 
    v4l2_capture.c;
    )

add_library(${CAMERA_DRIVER_TARGET} ${SRC_LIST})
