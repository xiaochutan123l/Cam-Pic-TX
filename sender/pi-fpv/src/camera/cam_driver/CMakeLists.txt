set(CAMERA_DRIVER_TARGET cam_driver_lib)

set(SRC_LIST 
    v4l2_capture.c;
    )

#dd_library(${CAMERA_DRIVER_TARGET} ${SRC_LIST})
if (NOT TARGET ${CAMERA_DRIVER_TARGET})
    add_library(${CAMERA_DRIVER_TARGET} ${SRC_LIST})
endif()