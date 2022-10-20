#include "cam_controller_impl.h"
#include <iostream>

#include "v4l2_capture.h"

Cam_Controller_Impl::Cam_Controller_Impl(Stream_fmt fmt, char *dev_name)
    : Cam_Controller(fmt, dev_name) {
    m_status = CAM_STATUS::IDLE;
    buffer_size = 4;
}

Cam_Controller_Impl::Cam_Controller_Impl(Stream_fmt fmt)
    : Cam_Controller(fmt){
    m_status = CAM_STATUS::IDLE;
    m_dev_name = nullptr;
    buffer_size = 4;
}

void Cam_Controller_Impl::start() {
    open_device(m_dev_name);
    m_status = CAM_STATUS::IDLE;
    init_device();
    // TODO: add determination here for the set result.ß
    _set_frame_format();
    _set_frame_rate();
    init_mmap(buffer_size);
    m_status = CAM_STATUS::INIT;
    start_capturing();
    m_status = CAM_STATUS::STARTED;
}

void Cam_Controller_Impl::_set_frame_format() {
    struct v4l2_pix_format set_fmt;
    switch(m_fmt.res_fmt){
        case RESOLUTION_FMT::R_240:
            set_fmt.width = 320;
            set_fmt.height = 240;
            break;
        case RESOLUTION_FMT::R_480:
            set_fmt.width = 720;
            set_fmt.height = 480;
            break;
        case RESOLUTION_FMT::R_720:
            set_fmt.width = 1280;
            set_fmt.height = 720;
            break;
        case RESOLUTION_FMT::R_1080:
            set_fmt.width = 1920;
            set_fmt.height = 1080;
            break;
        case RESOLUTION_FMT::R_2K:
            set_fmt.width = 2560;
            set_fmt.height = 1440;
            break;
    }
    
    //set_fmt.pixelformat = V4L2_PIX_FMT_YUV420;
    set_fmt.pixelformat = V4L2_PIX_FMT_YUYV;
    set_fmt.field = V4L2_FIELD_INTERLACED;
    set_frame_format(&set_fmt);
}

int Cam_Controller_Impl::_set_frame_rate() {
    struct v4l2_fract f_rate;
    f_rate.numerator = 1;
    if (m_fmt.frame_rate < 1)
        m_fmt.frame_rate = 1;
    f_rate.denominator = m_fmt.frame_rate;

    set_frame_rate(&f_rate);

    return f_rate.denominator;
}

void Cam_Controller_Impl::get_frame() {
    printf("cpp capture frame\n");
    struct v4l2_buffer v_buf;
    struct buffer buf;
    capture_frame(&v_buf, &buf);
    // TODO: do something, handle this frame.
    release_frame_buffer(&v_buf);
}

void Cam_Controller_Impl::stop() {
    stop_capturing();
}

void Cam_Controller_Impl::close() {
    switch(m_status){
        case CAM_STATUS::IDLE:
        case CAM_STATUS::CLOSED:
            break;
        case CAM_STATUS::OPENNED:
        case CAM_STATUS::INIT:
        case CAM_STATUS::STARTED:
            uninit_device();
            stop_capturing();
            close_device();
            m_status = CAM_STATUS::CLOSED;
            break;
        case CAM_STATUS::STOPPED:
            close_device();
            m_status = CAM_STATUS::CLOSED;
            break;
    }
}

void Cam_Controller_Impl::reset_format(Stream_fmt fmt) {
    // TODO: determine if the frame format was set to the desired value.
    _set_frame_format();
    // TODO: determine if the framerate was set to the desired value.
    _set_frame_rate();
    // TODO: test if the following two steps are neccesary.
    init_mmap(buffer_size);
    start_capturing();
}

Cam_Controller_Impl::~Cam_Controller_Impl() {
    close();
}