#include "fmt_utils.h"

__u32 to_v4l2_fmt(IMAGE_FMT fmt) {
    // https://docs.huihoo.com/doxygen/linux/kernel/3.7/uapi_2linux_2videodev2_8h_source.html
    switch(fmt) {
        case IMAGE_FMT::RGB24:
            return V4L2_PIX_FMT_RGB24;
        case IMAGE_FMT::JPEG:
            return V4L2_PIX_FMT_JPEG;
        case IMAGE_FMT::MJPG:
            return V4L2_PIX_FMT_MJPEG;
        /*
        case IMAGE_FMT::YUV444:
            return V4L2_PIX_FMT_H264;
        case IMAGE_FMT::YUV422:
            return V4L2_PIX_FMT_H264;
        */
        case IMAGE_FMT::H264:
            return V4L2_PIX_FMT_H264;
        case IMAGE_FMT::YUV420P:
            return V4L2_PIX_FMT_YUV420;
        default:
            return 0;
    }
}

AVPixelFormat to_ffmpeg_fmt(IMAGE_FMT fmt) {
    // https://ffmpeg.org/doxygen/4.1/pixfmt_8h.html#a9a8e335cf3be472042bc9f0cf80cd4c5
    switch(fmt) {
        case IMAGE_FMT::RGB24:
            return AV_PIX_FMT_RGB24 ;
        case IMAGE_FMT::YUV420P:
            return AV_PIX_FMT_YUV420P;
        default:
            printf("wrong format conversion from IMAGE_FMT to AVPixelFormat!\n");
            return AV_PIX_FMT_NONE;
    }
}