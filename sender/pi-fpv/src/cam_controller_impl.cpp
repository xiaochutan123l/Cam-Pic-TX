#include "cam_controller_impl.h"
#include <iostream>

//extern "C" {
//    #include "v4l2_capture.h"
//}

Cam_Controller_Impl::Cam_Controller_Impl(Stream_fmt fmt, char *dev_name)
    : Cam_Controller(fmt, dev_name) {
    m_status = CAM_STATUS::IDLE;
}

Cam_Controller_Impl::Cam_Controller_Impl(Stream_fmt fmt)
    : Cam_Controller(fmt){
    m_status = CAM_STATUS::IDLE;
    m_dev_name = nullptr;
}

void Cam_Controller_Impl::start() {
    open_device(m_dev_name);
    m_status = CAM_STATUS::IDLE;
    init_device();
    set_format();
    init_mmap();
    m_status = CAM_STATUS::INIT;
    start_capturing();
    m_status = CAM_STATUS::STARTED;
}

void Cam_Controller_Impl::capture_frame(int count) {
    capture_frame(count);
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
            stop_capturing();
            close_device();
            break;
        case CAM_STATUS::STOPPED:
            close_device();
            break;
    }
}

void Cam_Controller_Impl::reset_format(Stream_fmt fmt) {
    set_format();
    // TODO: test if the following two steps are neccesary.
    init_mmap();
    start_capturing();
}

Cam_Controller_Impl::~Cam_Controller_Impl() {
    close();
}