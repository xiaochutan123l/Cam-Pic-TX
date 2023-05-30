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
 * video decoding with libavcodec API example
 *
 * @example decode_video.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libavcodec/avcodec.h>

#define INBUF_SIZE 10240

static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
                     char *filename)
{
    FILE *f;
    int i;

    //f = fopen(filename,"wb");
    f = fopen(filename,"a");
    printf("write file\n");
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}

void yuv420p_save(unsigned char *buf, int wrap, int xsize, int ysize, char *filename) {
    FILE *f;
    int i;

    f = fopen(filename, "a");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);

    // /* Write Y plane */
    // for (i = 0; i < ysize; i++)
    //     fwrite(buf + i * wrap, 1, xsize, f);

    // /* Write U plane */
    // for (i = 0; i < ysize / 2; i++)
    //     fwrite(buf + wrap * ysize + i * wrap / 2, 1, xsize / 2, f);

    // /* Write V plane */
    // for (i = 0; i < ysize / 2; i++)
    //     fwrite(buf + wrap * ysize + wrap * ysize / 4 + i * wrap / 2, 1, xsize / 2, f);

    fclose(f);
}


static void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt,
                   const char *filename)
{
    char buf[1024];
    int ret;

    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error sending a packet for decoding\n");
        exit(1);
    }
    printf("send packet return: %d\n", ret);
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        printf("receive frame ret: %d\n", ret);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            printf("receive frame error:\n");
            return;
        }
        else if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }
        printf("save frame\n");
        printf("saving frame %3d\n", dec_ctx->frame_number);
        fflush(stdout);

        /* the picture is allocated by the decoder. no need to
           free it */
        snprintf(buf, sizeof(buf), "%s-%d", filename, dec_ctx->frame_number);
        printf("pgm_save: \n");
        // pgm_save(frame->data[0], frame->linesize[0],
        //         frame->width, frame->height, buf);

        // pgm_save(frame->data[1], frame->linesize[1],
        //         frame->width/2, frame->height/2, buf);

        // pgm_save(frame->data[2], frame->linesize[2],
        //         frame->width/2, frame->height/2, buf);
        yuv420p_save(frame->data[0], frame->linesize[0], frame->width, frame->height, buf);
        yuv420p_save(frame->data[1], frame->linesize[1], frame->width/2, frame->height/2, buf);
        yuv420p_save(frame->data[2], frame->linesize[2], frame->width/2, frame->height/2, buf);

    }
}

int main(int argc, char **argv)
{
    av_log_set_level(AV_LOG_DEBUG);
    const char *filename, *outfilename;
    const AVCodec *codec;
    AVCodecParserContext *parser;
    AVCodecContext *c= NULL;
    FILE *f;
    AVFrame *frame;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t *data;
    size_t   data_size;
    int ret;
    int eof;
    AVPacket *pkt;

    if (argc <= 2) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n"
                "And check your input file is encoded by mpeg1video please.\n", argv[0]);
        exit(0);
    }
    filename    = argv[1];
    outfilename = argv[2];

    pkt = av_packet_alloc();
    if (!pkt)
        exit(1);

    /* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    /* find the MPEG-1 video decoder */
    //codec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);
    //codec = avcodec_find_decoder_by_name("h264_v4l2m2m");
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    parser = av_parser_init(codec->id);
    if (!parser) {
        fprintf(stderr, "parser not found\n");
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    /* For some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because this information is not
       available in the bitstream. */

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    do {
        /* read raw data from the input file */
        data_size = fread(inbuf, 1, INBUF_SIZE, f);
        if (ferror(f))
            break;
        eof = !data_size;

        /* use the parser to split the data into frames */
        data = inbuf;
        while (data_size > 0 || eof) {
            ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size,
                                   data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
            if (ret < 0) {
                fprintf(stderr, "Error while parsing\n");
                exit(1);
            }
            data      += ret;
            data_size -= ret;

            if (pkt->size)
                decode(c, frame, pkt, outfilename);
            else if (eof)
                break;
        }
    } while (!eof);

    /* flush the decoder */
    decode(c, frame, NULL, outfilename);

    fclose(f);
    printf("close f\n");
    av_parser_close(parser);
    printf("av parser closr\n");
    avcodec_free_context(&c);
    printf("free context\n");
    av_frame_free(&frame);
    printf("frame free\n");
    av_packet_free(&pkt);

    return 0;
}