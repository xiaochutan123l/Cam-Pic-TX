#pragma once

#include "v4l2_capture.h"
#include "camera_config.h"
#include <libavcodec/avcodec.h>

__u32 to_v4l2_fmt(IMAGE_FMT fmt);

AVPixelFormat to_ffmpeg_fmt(IMAGE_FMT fmt);