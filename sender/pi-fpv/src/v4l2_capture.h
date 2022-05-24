#ifndef _CAMERA_CONTROLLER_
#define _CAMERA_CONTROLLER_

#include <cstddef>
#include <stdint.h>
//#define CLEAR(x) memset(&(x), 0, sizeof(x))

enum io_method {
        IO_METHOD_READ,
        IO_METHOD_MMAP,
        IO_METHOD_USERPTR,
};

struct buffer {
        void   *start;
        size_t  length;
};

class CameraController{

private:
    char             *dev_name;
    enum io_method   io;
    int              fd;
    //buffer           *buffers;
    unsigned int     n_buffers;
    int              out_buf;
    int              force_format;
    int              frame_count;

    void errno_exit(const char *s);
    int xioctl(int fh, int request, void *arg);
    void process_image(const void *p, int size);
    int read_frame();
    void init_read(unsigned int buffer_size);
    void init_mmap();
    void init_userp(unsigned int buffer_size);
public:
    buffer	     *buffers;
    CameraController();
    //~CameraController();

    void open_device();
    void init_device();
    void start_capturing();
    void capture_frame();
    void stop_capturing();
    void uninit_device();
    void close_device();
    //TODO:
    //set_cam_parameter();
};


#endif /*_CAMERA_CONTROLLER_*/
