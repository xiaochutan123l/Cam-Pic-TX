#pragma once 

#include "cam_controller_impl.h"
#include "camera_config.h"


class Cam_Controller {
public:
    
    Cam_Controller(Stream_fmt fmt, char *dev_name):m_fmt(fmt), m_dev_name(dev_name){};
    Cam_Controller(Stream_fmt fmt):m_fmt(fmt){};
    // initialize and start camera for capturing.
    virtual void start() = 0;
    // capture a frame.
    virtual void capture_frame(int count) = 0;
    virtual void reset_format(Stream_fmt fmt) = 0;

    virtual void stop() = 0;
    virtual void close() = 0;
    virtual ~Cam_Controller(){};
    //virtual Stream_fmt* getFMT();

protected:
    char *m_dev_name;
    Stream_fmt m_fmt;
    CAM_STATUS m_status;
};