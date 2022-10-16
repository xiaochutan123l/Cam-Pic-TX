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
	RGB3,
	JPEG,
	H264,
	MJPG,
	YUV444,
	YUV422,
	YUV420,
	// and more.
};

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

#endif /*_CAMERA_CONFIG_H_*/