#ifndef _V4L2_CAPTURE_H_
#define _V4L2_CAPTURE_H_

#include <linux/videodev2.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void open_device(char *dev_name);
extern void init_device();
extern void init_mmap();
extern void start_capturing();
extern void set_frame_format(struct v4l2_pix_format *set_fmt);
extern void set_frame_rate(struct v4l2_fract *s_parm);
extern void capture_frame(int count);

extern void stop_capturing();
extern void uninit_device();
extern void close_device();

#ifdef __cplusplus
}
#endif

#endif /*_V4L2_CAPTURE_H_*/
