#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/opt.h>
    #include <libavutil/imgutils.h>
}
#include "encoder_impl.h"

EncoderImpl::EncoderImpl() {
    init();
    size_t _frame_pts = 0;
    _codec_name = "libx264";
}

void EncoderImpl::init() {
    prepare_encoder();
    set_format();
    open_encoder();
}

void EncoderImpl::prepare_encoder() {
    _codec = avcodec_find_encoder_by_name(_codec_name);
    if (!_codec) {
        fprintf(stderr, "Codec '%s' not found\n", _codec_name);
        exit(1);
    }
    printf("find codec...\n");
    _enc_ctx = avcodec_alloc_context3(_codec);
    if (!_enc_ctx) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }
    printf("alloc...\n");
    _pkt = av_packet_alloc();
    if (!_pkt)
        exit(1);
    printf("alloc...ok\n");
}

void EncoderImpl::set_format() {
    /* put sample parameters */
    // 比特率会导致图像压缩更厉害，如果低比特率的话，解码出来比较模糊
    _enc_ctx->bit_rate = 2000000;
    /* resolution must be a multiple of two */
    //c->width = 352;
    //c->height = 288;
    _enc_ctx->width = 1280;
    _enc_ctx->height = 720;
    /* frames per second */
    _enc_ctx->time_base = (AVRational){1, 30};
    _enc_ctx->framerate = (AVRational){30, 1};

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder 
     * will always be I frame irrespective to gop_size
     */
    //c->gop_size = 5;
    _enc_ctx->gop_size = 5;
    _enc_ctx->max_b_frames = 0;
    //c->pix_fmt = AV_PIX_FMT_YUV420P;
    _enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

    if (_codec->id == AV_CODEC_ID_H264)
        //av_opt_set(c->priv_data, "preset", "slow", 0);
        //av_opt_set(c->priv_data, "preset", "superfast", 0);
        av_opt_set(_enc_ctx->priv_data, "tune", "zerolatency", 0);
}

void EncoderImpl::open_encoder() {
    printf("open codec...\n");
    /* open it */
    int ret = avcodec_open2(_enc_ctx, _codec, NULL);
    if (ret < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE];  // 创建一个足够大的缓冲区来存储错误消息
        av_strerror(ret, errbuf, AV_ERROR_MAX_STRING_SIZE);  // 将错误消息复制到缓冲区中
        fprintf(stderr, "Could not open codec: %s\n", errbuf);
        exit(1);
    }
    printf("open...ok\n");

    printf("frame alloc...\n");
    _frame = av_frame_alloc();
    if (!_frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    _frame->format = _enc_ctx->pix_fmt;
    _frame->width  = _enc_ctx->width;
    _frame->height = _enc_ctx->height;
    printf("frame buffer...\n");
    ret = av_frame_get_buffer(_frame, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }
}

void EncoderImpl::send_frame(const uint8_t *frame_buffer, size_t size) {
    // copy frame data
    int y_size = _enc_ctx->width * _enc_ctx->height;
    int u_size = y_size / 4;
    int v_size = y_size / 4;
    memcpy(_frame->data[0], frame_buffer, y_size);
    memcpy(_frame->data[1], frame_buffer + y_size, u_size);
    memcpy(_frame->data[2], frame_buffer + y_size + u_size, v_size);

    if (_frame)
        printf("Send frame %3" PRId64 "\n", _frame->pts);
    printf("avcodec send frame");
    if (avcodec_send_frame(_enc_ctx, _frame) < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }
}

int EncoderImpl::receive_packet(void *packet_buffer, size_t &size) {
    int ret = avcodec_receive_packet(_enc_ctx, _pkt);
    // AVERROR(EAGAIN): 如果编码器还没有准备好输出数据包（例如，因为它仍在处理帧或需要更多的帧才能开始输出
    // AVERROR_EOF: 如果编码器已经完全完成并且没有更多的数据包可用，那么它将返回
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        return 0;
    else if (ret < 0) {
        fprintf(stderr, "Error during encoding\n");
        exit(1);
    }
    printf("Write packet %3" PRId64 " (size=%5d)\n", _pkt->pts, _pkt->size);
    size = _pkt->size;
    memcpy(packet_buffer, _pkt->data, size);
    av_packet_unref(_pkt);
    // TODO: 或者直接返回pkt data指针给decoder直接memcpy
    return 1;
}




void EncoderImpl::close_encoder() {
    // flush encoder.
    avcodec_send_frame(_enc_ctx, NULL);
    avcodec_free_context(&_enc_ctx);
    av_frame_free(&_frame);
    av_packet_free(&_pkt);
    //fclose(out_file);
}

EncoderImpl::~EncoderImpl() {
    close_encoder();
}

// void EncoderImpl::open_in_file() {
//     FILE *file = fopen(in_file_name, "rb");
//     if (file == NULL) {
//         printf("无法打开文件\n");
//         return 1;
//     }
//     size_t ret = 0;
//     uint8_t *y_ptr = frame->data[0];
//     uint8_t *u_ptr = frame->data[1];
//     uint8_t *v_ptr = frame->data[2];
//     int y_size = c->width * c->height;
//     int u_size = y_size / 4;
//     int v_size = y_size / 4;
//     ret = fread(y_ptr, 1, y_size, file);
//     ret += fread(u_ptr, 1, u_size, file);
//     ret += fread(v_ptr, 1, v_size, file);
//     if (ret != y_size + u_size + v_size) {
//         printf("Failed to read input frame file size: %d, read size: %d\n", ret, (y_size + u_size + v_size));
//     }
//     fclose(file);
// }

// size_t EncoderImpl::encode() {
//     int ret;
//     size_t write_size = 0;
//     /* send the frame to the encoder */
//     if (frame)
//         printf("Send frame %3"PRId64"\n", frame->pts);
//     ret = avcodec_send_frame(c, frame);
//     printf("avcodec send frame");
//     if (ret < 0) {
//         fprintf(stderr, "Error sending a frame for encoding\n");
//         exit(1);
//     }

//     while (ret >= 0) {
//         ret = avcodec_receive_packet(c, pkt);
//         if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
//             return;
//         else if (ret < 0) {
//             fprintf(stderr, "Error during encoding\n");
//             exit(1);
//         }
//         printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
//         //fwrite(pkt->data, 1, pkt->size, out_file);

//         memcpy(packet_buffer + pkt->size, pkt->data, pkt->size);
//         write_size += pkt->size;

//         av_packet_unref(pkt);
//     }
//     return write_size;
// }

// void EncoderImpl::open_out_file() {
//     out_file = fopen(filename, "wb");
//     if (!out_file) {
//         fprintf(stderr, "Could not open %s\n", filename);
//         exit(1);
//     }
// }

// void EncoderImpl::start_encode() {
//     frame->pts = 0;
//     encode(c, frame, pkt, out_file);

//     printf("encode out loop...\n");
//     /* flush the encoder */
//     encode(c, NULL, pkt, out_file);
    
// }