#include "cam_controller_impl.h"
#include "cam_controller.h"
#include <iostream>
#include <string> 
#include <map>
#include <zxing/src/ReadBarcode.h>
#include <chrono>

//using namespace std;

/*
frame capture handler callback function.
*/
void handler_cb(void *start, uint32_t length) {
    printf("handler callback function, frame size: %d bytes\n", length);
	//FILE *out_fp = fopen("./test.jpg","w");
    // FILE *out_fp = fopen(filename.c_str(), "w");
	// if(out_fp==NULL) {
	// 	printf("File cannot be opened\n");
	// 	exit(EXIT_FAILURE);
	// }
	// else
	// {
	// 	printf("File opened for writing\n");
	// 	fwrite(start, length, 1, out_fp);
	// }
	// fclose(out_fp);
}

int main (int argc, char *argv[]) {
    uint8_t frame_rate = 30;
    RESOLUTION_FMT res_fmt = RESOLUTION_FMT::R_1080;
    IMAGE_FMT image_fmt = IMAGE_FMT::JPEG;
    char *dev_name = (char*)"/dev/video0";
    // default value.
    Stream_fmt fmt = {res_fmt, frame_rate, image_fmt};

    // start camera.
    Cam_Controller *cam = new Cam_Controller_Impl(fmt, dev_name);
    std::cout << "start cam ..." << std::endl;
    cam->start(handler_cb);
    //cam->start(handler_cb_fake);
    std::cout << "camera ready, wait for capturing" << std::endl;

    // warm up camera.
    for (int i = 0; i < 5; i++){
        cam->get_frame();
    }

    // test frame capture.
    cam->get_frame();

    cam->close();
    std::cout << "exit" << std::endl;
    delete cam;
 
    return 0;
}