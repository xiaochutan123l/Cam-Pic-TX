#include "cam_controller_impl.h"
#include "cam_controller.h"
#include <iostream>
#include <string>

using namespace std;
uint8_t frame_rate = 30;

int main_arg_count = 2;

std::string info = 
        "Usage: stream_cap_test [options] [flags]\n\n"
        "options:\n"
        "--single      capture a single frame\n"
        "--multi       capture multiple frames\n"
        "\nflags:\n"
        "-o            output file name/base name\n"
        "-rate         capture frame rate"

void handler_cb(void *start, uint32_t length) {
    printf("handler callback function\n");
	FILE *out_fp = fopen("./test.jpg","w");
	if(out_fp==NULL) {
		printf("File cannot be opened\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("File opened for writing\n");
		fwrite(start, length, 1, out_fp);

	}
	fclose(out_fp);
}

void single_frame_test() {
    char *dev_name = (char*)"/dev/video0";
    Stream_fmt fmt = {RESOLUTION_FMT::R_240,
                        frame_rate,
                        IMAGE_FMT::JPEG};

    Cam_Controller *cam = new Cam_Controller_Impl(fmt, dev_name);
    cout << "start cam ..." << endl;
    cam->start(handler_cb);
    cout << "camera ready, wait for capturing" << endl;
    cam->get_frame();
    cout << "capture done" << endl;
    cam->close();
    cout << "exit" << endl;
    delete cam;
}

void capture_multiple_frame() {
    char *dev_name = (char*)"/dev/video0";
    Stream_fmt fmt = {RESOLUTION_FMT::R_240,
                        frame_rate,
                        IMAGE_FMT::JPEG};
                        
    Cam_Controller *cam = new Cam_Controller_Impl(fmt, dev_name);
    cout << "start cam ..." << endl;
    cam->start(handler_cb);
    cout << "camera ready, wait for capturing" << endl;

    for (int i = 0; i < count; i++){
        cam->get_frame();
    }

    cout << "capture done" << endl;
    cam->close();
    cout << "exit" << endl;
    delete cam;

}

int main (int argc, char *argv[]) {
    if (argc <= 2) {
        fprintf(stderr, "Usage: %s <output file> <codec name>\n", argv[0]);
        exit(0);
    }

 
    return 1;
}