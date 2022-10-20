/*
 *  V4L2 video capture example
 *
 *  This program can be used and distributed without restrictions.
 *
 *      This program is provided with the V4L2 API
 * see https://linuxtv.org/docs.php for more information
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

//#include <linux/videodev2.h>
#include "v4l2_capture.h"
//#include <time.h>
//#include <cstddef>
//#include <stdint.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

static char             *dev_name;
static int              fd;
static struct buffer    *buffers;
static unsigned int     n_buffers;

void errno_exit(const char *s)
{
        fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
        exit(EXIT_FAILURE);
}

/*
void process_image(const void *p, int size)
{	
	//fwrite(p, size, 1, stdout);
	printf("hahaha\n");
        //if (out_buf){
	//uint8_t *new_buffer = (uint8_t*)malloc(size);
	//memcpy(new_buffer, p, size);
	FILE *out_fp = fopen("./test.yuv","w");
	if(out_fp==NULL) {
		printf("File cannot be opened\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("File opened for writing\n");
		//fwrite(new_buffer, size, 1, out_fp);
		fwrite(p, size, 1, out_fp);

	}
	//fwrite(p, size, 1, out_fp);
	fclose(out_fp);
	//}
        //fflush(stderr);
        //fflush(stdout);
}
*/

int read_frame(struct v4l2_buffer *v_buf, struct buffer *buf)
{
        // 在 driver 内部管理着两个 buffer queues ，一个输入队列，一个输出队列。
        // 对于 capture device 来说，当输入队列中的 buffer被塞满数据以后会自动变为输出队列，
        // 等待调用 VIDIOC_DQBUF(从队列中取出帧) 将数据进行处理以后重新调用 VIDIOC_QBUF (把帧放入队列)将 buffer 重新放进输入队列.
        // ！！！ 在一个buf从出列队中取出，在再次放回去之前内容不会被覆盖，不用担心driver和程序同时写入和读取同一个buffer问题。
        // ！！！ 所以必须保证处理buffer的时间足够快，在输入队列空之前将使用完的buffer重新入列，不然就会导致丢失图像帧。
        
        /* 从视频采集输入队列取出帧缓冲区 */
        if (-1 == ioctl(fd, VIDIOC_DQBUF, v_buf)) {
                fprintf(stderr, "Fail to ioctl 'VIDIOC_DQBUF'\n");
                errno_exit("VIDIOC_DQBUF");
        }
        assert(v_buf->index < n_buffers);
        //process_image(buffers[v_buf->index].start, buf->bytesused);
        buf->start = buffers[v_buf->index].start,
        buf->length = v_buf->bytesused;
        return 1;
}

void release_frame_buffer(struct v4l2_buffer *v_buf) {
        /* 把用用完的缓冲帧放回队列 */
        if (-1 == ioctl(fd, VIDIOC_QBUF, v_buf))
                errno_exit("VIDIOC_QBUF");
}

/*
v_buf: v4l2_buffer
buf: frame buffer
*/
// capture a single frame each time.
void capture_frame(struct v4l2_buffer *v_buf, struct buffer *buf)
{
        CLEAR(*v_buf);
        v_buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v_buf->memory = V4L2_MEMORY_MMAP;
        for (;;) {
                fd_set fds;
                struct timeval tv;
                int r;
                // 将指定的文件描述符集清空
                FD_ZERO(&fds);
                // 在文件描述符集合中增加一个新的文件描述符
                FD_SET(fd, &fds);

                /* Timeout. */
                tv.tv_sec = 2;
                tv.tv_usec = 0;
                // 判断是否可读(即摄像头是否准备好)，tv是定时
                r = select(fd + 1, &fds, NULL, NULL, &tv);
                if (-1 == r) {
                        if (EINTR == errno)
                                continue;
                        errno_exit("select");
                }
                if (0 == r) {
                        fprintf(stderr, "select timeout\n");
                        exit(EXIT_FAILURE);
                }
                if (read_frame(v_buf, buf))
                        break;
                /* EAGAIN - continue select loop. */
        }
}

void stop_capturing()
{
        /* 关闭视频流 */
        enum v4l2_buf_type type;
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == ioctl(fd, VIDIOC_STREAMOFF, &type))
                errno_exit("VIDIOC_STREAMOFF");
}

void start_capturing()
{
        unsigned int i;
        enum v4l2_buf_type type;

        for (i = 0; i < n_buffers; ++i) {
                struct v4l2_buffer buf;

                CLEAR(buf);
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = i;
                // 将申请到的帧缓冲放入缓存队列
                if (-1 == ioctl(fd, VIDIOC_QBUF, &buf))
                        errno_exit("VIDIOC_QBUF");
        }
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        /* 打开采集视频 */
        if (-1 == ioctl(fd, VIDIOC_STREAMON, &type))
                errno_exit("VIDIOC_STREAMON");
}

void init_mmap(int buffer_num)
{
        struct v4l2_requestbuffers req;
        CLEAR(req);
        req.count = buffer_num;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;
        // 请求缓冲区
        if (-1 == ioctl(fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        fprintf(stderr, "%s does not support "
                                 "memory mapping\n", dev_name);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_REQBUFS");
                }
        }

        if (req.count < 2) {
                fprintf(stderr, "Insufficient buffer memory on %s\n",
                         dev_name);
                exit(EXIT_FAILURE);
        }

        buffers = (struct buffer*)calloc(req.count, sizeof(struct buffer));

        if (!buffers) {
                fprintf(stderr, "Out of memory\n");
                exit(EXIT_FAILURE);
        }

        for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
                struct v4l2_buffer buf;
                CLEAR(buf);
                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = n_buffers;
                // 查询序号为n_buffer的缓冲区，得到其起始物理地址和大小
                if (-1 == ioctl(fd, VIDIOC_QUERYBUF, &buf))
                        errno_exit("VIDIOC_QUERYBUF");

                buffers[n_buffers].length = buf.length;
                buffers[n_buffers].start =
                        mmap(NULL /* start anywhere */,
                              buf.length,
                              PROT_READ | PROT_WRITE /* required */,
                              MAP_SHARED /* recommended */,
                              fd, buf.m.offset);
                if (MAP_FAILED == buffers[n_buffers].start)
                        errno_exit("mmap");
        }
}

/*
 * check device capabilities
*/
void init_device()
{
        // int ioctl(int fd, ind cmd, …)； 
        // ioctl 可以对设备进行控制.
        struct v4l2_capability cap;
        // 查询设备属性
        if (-1 == ioctl(fd, VIDIOC_QUERYCAP, &cap)) {
                if (EINVAL == errno) {
                        fprintf(stderr, "%s is no V4L2 device\n",
                                 dev_name);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_QUERYCAP");
                }
        }
        // 确认设备支持视频采集
        if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
                fprintf(stderr, "%s is no video capture device\n",
                         dev_name);
                exit(EXIT_FAILURE);
        }

        if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                fprintf(stderr, "%s does not support streaming i/o\n",
                                dev_name);
                exit(EXIT_FAILURE);
        }
}

void uninit_device()
{
        unsigned int i;
        for (i = 0; i < n_buffers; ++i)
                if (-1 == munmap(buffers[i].start, buffers[i].length))
                        errno_exit("munmap");
        free(buffers);
}

void set_frame_format(struct v4l2_pix_format *set_fmt) {
        // 设置图像格式
        struct v4l2_format fmt;
        unsigned int min;
        CLEAR(fmt);
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        
        fmt.fmt.pix.width       = set_fmt->width;
        fmt.fmt.pix.height      = set_fmt->height;
        fmt.fmt.pix.pixelformat = set_fmt->pixelformat;
        fmt.fmt.pix.field       = set_fmt->field;
        // set format.
        if (-1 == ioctl(fd, VIDIOC_S_FMT, &fmt))
                errno_exit("VIDIOC_S_FMT");

        /* Note VIDIOC_S_FMT may change width and height. */
        /* Preserve original settings as set by v4l2-ctl for example */
        if (-1 == ioctl(fd, VIDIOC_G_FMT, &fmt))
                errno_exit("VIDIOC_G_FMT");

        // That should allow setting the capture-side frame rate. 
        // It's your task to make sure you're reading fast enough not to get frame drops.
        set_fmt->width = fmt.fmt.pix.width;
        set_fmt->height = fmt.fmt.pix.height;
        
}

void set_frame_rate(struct v4l2_fract *s_parm) {
        // 设置图像格式
        struct v4l2_streamparm streamparm;
        CLEAR(streamparm);
        streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == ioctl(fd, VIDIOC_G_PARM, &streamparm)) {
            errno_exit("VIDIOC_G_PARM");
        } 
        streamparm.parm.capture.capturemode |= V4L2_CAP_TIMEPERFRAME;
        streamparm.parm.capture.timeperframe.numerator = s_parm->numerator;
        streamparm.parm.capture.timeperframe.denominator = s_parm->denominator;
        // set format.
        // VIDIOC_G_PARM : G -> get , S -> set.
        if (-1 == ioctl(fd, VIDIOC_S_PARM, &streamparm)){
            errno_exit("VIDIOC_S_PARM");
        } 
        if (-1 == ioctl(fd, VIDIOC_G_PARM, &streamparm)) {
                errno_exit("VIDIOC_G_PARM");
        }

        s_parm->numerator = streamparm.parm.capture.timeperframe.numerator;
        s_parm->denominator = streamparm.parm.capture.timeperframe.denominator;
}

void close_device()
{
        if (-1 == close(fd))
                errno_exit("close");

        fd = -1;
}

void open_device(char *dev_name)
{
        struct stat st;

        if (-1 == stat(dev_name, &st)) {
                fprintf(stderr, "Cannot identify '%s': %d, %s\n",
                                dev_name, errno, strerror(errno));
                exit(EXIT_FAILURE);
        }

        if (!S_ISCHR(st.st_mode)) {
                fprintf(stderr, "%s is no device\n", dev_name);
                exit(EXIT_FAILURE);
        }

        fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

        if (-1 == fd) {
                fprintf(stderr, "Cannot open '%s': %d, %s\n",
                                dev_name, errno, strerror(errno));
                exit(EXIT_FAILURE);
        }
}
