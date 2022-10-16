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

#include <linux/videodev2.h>
#include "v4l2_capture.h"
//#include <time.h>
#include <cstddef>
#include <stdint.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

struct buffer {
        void   *start;
        size_t  length;
};

static char             *dev_name;
static int              fd;
static buffer           *buffers;
static unsigned int     n_buffers;

void errno_exit(const char *s)
{
        fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
        exit(EXIT_FAILURE);
}

void process_image(const void *p, int size)
{	
	//fwrite(p, size, 1, stdout);
	printf("hahaha\n");
        //if (out_buf){
	//uint8_t *new_buffer = (uint8_t*)malloc(size);
	//memcpy(new_buffer, p, size);
	FILE *out_fp = fopen("./test.jpg","w");
	
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

int read_frame()
{
        struct v4l2_buffer buf;
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        /* 从视频采集输入队列取出帧缓冲区 */
        if (-1 == ioctl(fd, VIDIOC_DQBUF, &buf)) {
                fprintf(stderr, "Fail to ioctl 'VIDIOC_DQBUF'\n");
                errno_exit("VIDIOC_DQBUF");
        }
        assert(buf.index < n_buffers);
        process_image(buffers[buf.index].start, buf.bytesused);
        /* 把用用完的缓冲帧放回队列 */
        if (-1 == ioctl(fd, VIDIOC_QBUF, &buf))
                errno_exit("VIDIOC_QBUF");

        return 1;
}

void capture_frame(int count)
{
        while (count-- > 0) {
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
			if (read_frame())
                                break;
                        /* EAGAIN - continue select loop. */
                }
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

void init_mmap()
{
        struct v4l2_requestbuffers req;

        CLEAR(req);

        req.count = 4;
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

        buffers = (buffer*)calloc(req.count, sizeof(buffer));

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

void set_format() {
        // 设置图像格式
        struct v4l2_format fmt;
        unsigned int min;
        CLEAR(fmt);
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bool force_format = true;
        if (force_format) {
                fmt.fmt.pix.width       = 640;
                fmt.fmt.pix.height      = 480;
                
		//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
                fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG;
		fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
                // set format.
                if (-1 == ioctl(fd, VIDIOC_S_FMT, &fmt))
                        errno_exit("VIDIOC_S_FMT");

                /* Note VIDIOC_S_FMT may change width and height. */
        } else {
                /* Preserve original settings as set by v4l2-ctl for example */
                if (-1 == ioctl(fd, VIDIOC_G_FMT, &fmt))
                        errno_exit("VIDIOC_G_FMT");
        }
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
