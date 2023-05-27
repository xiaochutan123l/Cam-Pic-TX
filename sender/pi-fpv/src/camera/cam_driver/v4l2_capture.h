#ifndef _V4L2_CAPTURE_H_
#define _V4L2_CAPTURE_H_

#include <linux/videodev2.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct buffer {
        void   *start;
        uint32_t length;
};

// define callback function for processing frame after capturing.
typedef void (*frame_handler_cb)(void *start, uint32_t length);


extern void open_device(char *dev_name);
extern void init_device();
extern void init_mmap(int buffer_num);
extern void start_capturing();

extern void set_frame_format(struct v4l2_pix_format *set_fmt);
extern void set_frame_rate(struct v4l2_fract *s_parm);
// TODO: add capture_frame with parameter(direct rerturn frame buffer ptr and length) variant back, 
// if later neccessary. 同时提供callback版本和直接返回frame buffer地址版本以供不同设备或不同需求版本。
//extern void capture_frame(struct v4l2_buffer *v_buf, struct buffer *buf);
extern void capture_frame();
extern void register_frame_handler_cb(frame_handler_cb cb);

extern void stop_capturing();
extern void uninit_device();
extern void close_device();

extern void set_encode_control();

#ifdef __cplusplus
}
#endif

#endif /*_V4L2_CAPTURE_H_*/
