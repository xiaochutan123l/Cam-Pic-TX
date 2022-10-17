#include "cam_controller_impl.h"
#include "cam_controller.h"
#include <iostream>

using namespace std;
uint8_t frame_rate = 30;

int main () {
    char *dev_name = (char*)"/dev/video0";
    Stream_fmt fmt = {RESOLUTION_FMT::R_240,
                        frame_rate,
                        IMAGE_FMT::YUV420};
    Cam_Controller *cam = new Cam_Controller_Impl(fmt, dev_name);
    cout << "start cam ..." << endl;
    cam->start();
    cout << "camera ready, wait for capturing" << endl;
    cam->get_frame(1);
    cout << "capture done" << endl;
    cam->close();
    cout << "exit" << endl;
    delete cam;
    return 1;
}