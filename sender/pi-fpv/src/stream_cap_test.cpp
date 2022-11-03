#include "cam_controller_impl.h"
#include "cam_controller.h"
#include <iostream>
#include <string>
#include <map>

//using namespace std;
uint8_t frame_rate = 30;

int main_arg_count = 2;

std::string filename;

std::string info = 
        "Usage: stream_cap_test [options] [flags]\n\n"
        "options:\n"
        "--frame_num      capture a single frame\n"
        "--multi          capture multiple frames\n"
        "\nflags:\n"
        "-o               output file name/base name, without .format\n"
        "-fmt             image format:\n"
        "              	    RGB,\n"
	    "                   JPEG,\n"
	    "                   MJPG,\n"
	    "                   YUV\n"
        "-rate           capture frame rate\n"
        "-res            resolution:\n"
        "                  240,\n"
        "                  480,\n"
        "                  720,\n"
        "                  1080\n";

std::map<std::string, RESOLUTION_FMT> res_map { {"240", RESOLUTION_FMT::R_240}, 
                                    {"480", RESOLUTION_FMT::R_480}, 
                                    {"720", RESOLUTION_FMT::R_720}, 
                                    {"1080", RESOLUTION_FMT::R_1080}};

IMAGE_FMT parse_fmt(std::string fmt, std::string &filename_ext) {
    if (fmt == "RGB") {
        filename_ext = ".rgb";
        return IMAGE_FMT::RGB24;
    }
    else if (fmt == "JPEG") {
        filename_ext = ".jpg";
        return IMAGE_FMT::JPEG;
    }         
    else if (fmt == "MJPG") {
        filename_ext = ".mjpg";
        return IMAGE_FMT::MJPG;
    }
    else if (fmt == "YUV") {
        filename_ext = ".yuv";
        return IMAGE_FMT::YUV420P;
    }  
    else {
        std::cerr << "wrong flag " << fmt << "or wrong sequence" << std::endl;
        std::cerr << info << std::endl;
        exit(0);
    }
}

RESOLUTION_FMT parse_resolution(std::string res) {
    if (res_map.find(res) != res_map.end())
        return res_map[res];

    std::cerr << "wrong flag: " << res << "or wrong sequence" << std::endl;
    std::cerr << info << std::endl;
    exit(0);
}

uint8_t parse_frame_rate(std::string rate) {
    if (std::stoi(rate) < 120)
        return (uint8_t)std::stoi(rate);
    std::cerr << "wrong flag: " << rate << " is not a valid number, max 120" << std::endl;
    std::cerr << info << std::endl;
    exit(0);
}

int parse_frame_num(std::string num) {
    if (std::stoi(num) < 1000)
        return std::stoi(num);
    std::cerr << "wrong flag: " << num << " is not a valid number, max 1000" << std::endl;
    std::cerr << info << std::endl;
    exit(0);
}


/*
frame capture handler callback function.
*/
void handler_cb(void *start, uint32_t length) {
    printf("handler callback function\n");
	//FILE *out_fp = fopen("./test.jpg","w");
    FILE *out_fp = fopen(filename.c_str(), "w");
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

void handler_cb_fake(void *start, uint32_t length) {
    std::cout << "write file: " << filename << std::endl;
}

int main (int argc, char *argv[]) {
    std::string filename_base = "frame";
    std::string filename_ext = ".jpg";
    int frame_count = 1;
    char *dev_name = (char*)"/dev/video0";
    // default value.
    Stream_fmt fmt = {RESOLUTION_FMT::R_240,
                        frame_rate,
                        IMAGE_FMT::JPEG};

    if (argc < 2) {
        std::cerr << info << std::endl;
        exit(0);
    }

    std::string option = argv[1];
    if (option == "--frame_num")
        frame_count = parse_frame_num(argv[2]);
    else if (option == "--frame_num" || option == "-h") {
        std::cerr << info << std::endl;
        return 0;
    }
    else {
        std::cerr << "Undefined option " << option << std::endl;
        std::cerr << info << std::endl;
        exit(0);
    }

    for (int i = 3; i < argc; i += 2) {
        std::string flag = argv[i];
        std::string value = argv[i+1];
        if (flag == "-o")
            filename_base = value;
        else if (flag == "-fmt")
            fmt.image_fmt = parse_fmt(value, filename_ext);
        else if (flag == "-rate")
            fmt.frame_rate = parse_frame_rate(value);
        else if (flag == "-res")
                fmt.res_fmt = parse_resolution(value);
        else {
            std::cerr << "Undefined option " << flag << std::endl;
            std::cerr << info << std::endl;
            exit(0);
        }
    }

    // start camera.
    Cam_Controller *cam = new Cam_Controller_Impl(fmt, dev_name);
    std::cout << "start cam ..." << std::endl;
    cam->start(handler_cb);
    //cam->start(handler_cb_fake);
    std::cout << "camera ready, wait for capturing" << std::endl;

    for (int i = 0; i < frame_count; i++){
        filename = filename_base + std::to_string(i) + filename_ext;
        cam->get_frame();
    }

    std::cout << "capture done" << std::endl;
    cam->close();
    std::cout << "exit" << std::endl;
    delete cam;
 
    return 1;
}