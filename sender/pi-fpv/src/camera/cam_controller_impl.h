#pragma once 

#include "cam_controller.h"
#include "v4l2_capture.h"

class Cam_Controller_Impl : public Cam_Controller{
public:
    Cam_Controller_Impl(Stream_fmt fmt, char *dev_name);
    Cam_Controller_Impl(Stream_fmt fmt);
    
    void start() override;
    void get_frame() override;
    void stop() override;
    void close() override;

    void reset_format(Stream_fmt fmt) override;
    //Stream_fmt* getFMT() override;

    ~Cam_Controller_Impl();

private:
    void _set_frame_format();
    int _set_frame_rate();
    int buffer_size;
};