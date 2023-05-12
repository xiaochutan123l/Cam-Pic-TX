#ifndef _CAMERA_CONFIG_H_
#define _CAMERA_CONFIG_H_
#include <stdint.h>
/*
Camera configuration enums. 
*/

enum class CAP_TYPE {
	IMAGE,
	STREAM,
};

enum class IMAGE_FMT {
	// cmd: v4l2-ctl --list-formats-ext to see what format are supported.
	RGB24,
	JPEG,
	//H264,
	MJPG,
	//YUV444,
	//YUV422,
	YUV420P,
	// and more.
};
/*
ioctl: VIDIOC_ENUM_FMT
        Type: Video Capture

        [0]: 'YU12' (Planar YUV 4:2:0)
        [1]: 'YUYV' (YUYV 4:2:2)
        [2]: 'RGB3' (24-bit RGB 8-8-8)
        [3]: 'JPEG' (JFIF JPEG, compressed)
        [4]: 'H264' (H.264, compressed)
        [5]: 'MJPG' (Motion-JPEG, compressed)
        [6]: 'YVYU' (YVYU 4:2:2)
        [7]: 'VYUY' (VYUY 4:2:2)
        [8]: 'UYVY' (UYVY 4:2:2)
        [9]: 'NV12' (Y/CbCr 4:2:0)
        [10]: 'BGR3' (24-bit BGR 8-8-8)
        [11]: 'YV12' (Planar YVU 4:2:0)
        [12]: 'NV21' (Y/CrCb 4:2:0)
        [13]: 'RX24' (32-bit XBGR 8-8-8-8)
*/
enum class RESOLUTION_FMT {
	// 320x240
	R_240,
	// 720x480 or 640x480
	R_480,
	// 1280x720
	R_720,
	// 1920×1080
	R_1080,
	// 2560x1440
	R_2K,
};

typedef struct stream_fmt{
	RESOLUTION_FMT res_fmt;
	uint8_t frame_rate;
	IMAGE_FMT image_fmt;
} Stream_fmt;

enum class CAM_STATUS {
	// idle status, nothing happened to camera.
	IDLE,
	// camera device is openned.
	OPENNED,
	// camera was initialized.
	INIT,
	// camera was started and ready for capturing.
	STARTED,
	// camera capturing was stopped.
	STOPPED,
	// camera device was stopped.
	CLOSED,
};

// handler function for interface side.
typedef void (*frame_handler_cb_i)(void *start, uint32_t length);

#endif /*_CAMERA_CONFIG_H_*/