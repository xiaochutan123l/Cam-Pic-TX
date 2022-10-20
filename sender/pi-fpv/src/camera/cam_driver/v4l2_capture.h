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

extern void open_device(char *dev_name);
extern void init_device();
extern void init_mmap(int buffer_num);
extern void start_capturing();
extern void set_frame_format(struct v4l2_pix_format *set_fmt);
extern void set_frame_rate(struct v4l2_fract *s_parm);
extern void capture_frame(struct v4l2_buffer *v_buf, struct buffer *buf);
extern void release_frame_buffer(struct v4l2_buffer *v_buf);
extern void stop_capturing();
extern void uninit_device();
extern void close_device();

#ifdef __cplusplus
}
#endif

#endif /*_V4L2_CAPTURE_H_*/
