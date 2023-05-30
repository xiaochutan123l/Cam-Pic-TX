/*
 * Copyright (c) 2001 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file
 * video encoding with libavcodec API example
 *
 * @example encode_video.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libavcodec/avcodec.h>

#include <libavutil/opt.h>
#include <libavutil/imgutils.h>

static void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                   FILE *outfile)
{
    int ret;

    /* send the frame to the encoder */
    if (frame)
        printf("Send frame %3"PRId64"\n", frame->pts);

    ret = avcodec_send_frame(enc_ctx, frame);
    printf("avcodec send frame");
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }

        printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}

int main(int argc, char **argv)
{
    const char *filename, *codec_name;
    const AVCodec *codec;
    AVCodecContext *c= NULL;
    int i, ret, x, y;
    FILE *f;
    AVFrame *frame;
    AVPacket *pkt;
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };

    if (argc <= 2) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        exit(0);
    }
    const char *in_file_name = argv[1];
    filename = argv[2];
    //codec_name = argv[2];

    codec_name = "libx264";
    printf("trying to find codec...\n");

    /* find the mpeg1video encoder */
    codec = avcodec_find_encoder_by_name(codec_name);
    if (!codec) {
        fprintf(stderr, "Codec '%s' not found\n", codec_name);
        exit(1);
    }
    printf("find codec...\n");
    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }
    printf("alloc...\n");
    pkt = av_packet_alloc();
    if (!pkt)
        exit(1);
    printf("alloc...ok\n");
    /* put sample parameters */
    // 比特率会导致图像压缩更厉害，如果低比特率的话，解码出来比较模糊
    c->bit_rate = 4000000;
    /* resolution must be a multiple of two */
    //c->width = 352;
    //c->height = 288;
    c->width = 1280;
    c->height = 720;
    /* frames per second */
    c->time_base = (AVRational){1, 30};
    c->framerate = (AVRational){30, 1};

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    //c->gop_size = 5;
    c->gop_size = 1;
    //c->max_b_frames = 1;
    //c->pix_fmt = AV_PIX_FMT_YUV420P;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec->id == AV_CODEC_ID_H264)
        //av_opt_set(c->priv_data, "preset", "slow", 0);
        //av_opt_set(c->priv_data, "preset", "superfast", 0);
        av_opt_set(c->priv_data, "tune", "zerolatency", 0);
    printf("open codec...\n");
    /* open it */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open codec: %s\n", av_err2str(ret));
        exit(1);
    }
    printf("open...ok\n");
    f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }
    printf("frame alloc...\n");
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;
    printf("frame buffer...\n");
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }
    printf("encode...\n");
    /* encode 1 second of video */
    // for (i = 0; i < 25; i++) {
    //     fflush(stdout);

    //     /* Make sure the frame data is writable.
    //        On the first round, the frame is fresh from av_frame_get_buffer()
    //        and therefore we know it is writable.
    //        But on the next rounds, encode() will have called
    //        avcodec_send_frame(), and the codec may have kept a reference to
    //        the frame in its internal structures, that makes the frame
    //        unwritable.
    //        av_frame_make_writable() checks that and allocates a new buffer
    //        for the frame only if necessary.
    //      */
    //     ret = av_frame_make_writable(frame);
    //     if (ret < 0)
    //         exit(1);

    //     /* Prepare a dummy image.
    //        In real code, this is where you would have your own logic for
    //        filling the frame. FFmpeg does not care what you put in the
    //        frame.
    //      */
    //     /* Y */
    //     for (y = 0; y < c->height; y++) {
    //         for (x = 0; x < c->width; x++) {
    //             frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
    //         }
    //     }

    //     /* Cb and Cr */
    //     for (y = 0; y < c->height/2; y++) {
    //         for (x = 0; x < c->width/2; x++) {
    //             frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
    //             frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
    //         }
    //     }

    //     frame->pts = i;

    //     /* encode the image */
    //     encode(c, frame, pkt, f);
    // }

    //在frame结构体中，data是一个指针数组，用于存储YUV分量的指针。data[0]指向Y分量的起始地址，data[1]指向U分量的起始地址，data[2]指向V分量的起始地址。通过这些指针，我们可以访问和操作帧的YUV数据。

    // 打开文件
    FILE *file = fopen(in_file_name, "rb");
    if (file == NULL) {
        printf("无法打开文件\n");
        return 1;
    }

    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL) {
        printf("内存分配失败\n");
        return 1;
    }

    // // 读取文件内容到缓冲区
    // size_t result = fread(buffer, 1, file_size, file);
    // if (result != file_size) {
    //     printf("读取文件失败\n");
    //     return 1;
    // }
    // Read YUV data from input file
    // Assuming planar YUV420 format with 8-bit components
    uint8_t *y_ptr = frame->data[0];
    uint8_t *u_ptr = frame->data[1];
    uint8_t *v_ptr = frame->data[2];
    int y_size = c->width * c->height;
    int u_size = y_size / 4;
    int v_size = y_size / 4;
    ret = fread(y_ptr, 1, y_size, file);
    ret += fread(u_ptr, 1, u_size, file);
    ret += fread(v_ptr, 1, v_size, file);
    if (file_size != y_size + u_size + v_size) {
        printf("Failed to read input frame file size: %d, read size: %d\n", file_size, (y_size + u_size + v_size));
    }

    // 在缓冲区末尾添加字符串结束符
    //buffer[file_size] = '\0';

    

    fflush(stdout);

    //frame->data = buffer;
    //memcpy(frame->data, buffer, result);
    frame->pts = 0;
    encode(c, frame, pkt, f);

    printf("encode out loop...\n");
    /* flush the encoder */
    encode(c, NULL, pkt, f);
    // 关闭文件
    fclose(file);
    /* Add sequence end code to have a real MPEG file.
       It makes only sense because this tiny examples writes packets
       directly. This is called "elementary stream" and only works for some
       codecs. To create a valid file, you usually need to write packets
       into a proper file format or protocol; see muxing.c.
     */
    if (codec->id == AV_CODEC_ID_MPEG1VIDEO || codec->id == AV_CODEC_ID_MPEG2VIDEO)
        fwrite(endcode, 1, sizeof(endcode), f);
    fclose(f);

    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    // free file read buffer
    free(buffer);
    return 0;
}