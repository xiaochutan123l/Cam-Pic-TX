#pragma once

#include <iostream>
#include <vector>
#include <string>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

class Encoder {
public:
    Encoder(const std::string& codecName, int width, int height, AVRational framerate, int bitrate) {
        codec = avcodec_find_encoder_by_name(codecName.c_str());
        if (!codec) {
            std::cerr << "Codec '" << codecName << "' not found" << std::endl;
            exit(1);
        }

        c = avcodec_alloc_context3(codec);
        if (!c) {
            std::cerr << "Could not allocate video codec context" << std::endl;
            exit(1);
        }

        c->bit_rate = bitrate;
        c->width = width;
        c->height = height;
        c->time_base = framerate;
        c->framerate = framerate;

        if (codec->id == AV_CODEC_ID_H264)
            av_opt_set(c->priv_data, "preset", "slow", 0);

        if (avcodec_open2(c, codec, NULL) < 0) {
            std::cerr << "Could not open codec" << std::endl;
            exit(1);
        }
    }

    ~Encoder() {
        avcodec_free_context(&c);
    }

    AVPacket* encodeFrame(AVFrame* frame) {
        AVPacket* pkt = av_packet_alloc();
        if (!pkt)
            exit(1);

        int ret = avcodec_send_frame(c, frame);
        if (ret < 0) {
            std::cerr << "Error sending a frame for encoding" << std::endl;
            exit(1);
        }

        ret = avcodec_receive_packet(c, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return nullptr;
        else if (ret < 0) {
            std::cerr << "Error during encoding" << std::endl;
            exit(1);
        }

        return pkt;
    }

    // Setters and getters
    void setBitrate(int bitrate) {
        c->bit_rate = bitrate;
    }

    int getBitrate() const {
        return c->bit_rate;
    }

    void setWidth(int width) {
        c->width = width;
    }

    int getWidth() const {
        return c->width;
    }

    void setHeight(int height) {
        c->height = height;
    }

    int getHeight() const {
        return c->height;
    }

    void setTimeBase(AVRational timebase) {
        c->time_base = timebase;
        c->framerate = timebase;
    }

    AVRational getTimeBase() const {
        return c->time_base;
    }

private:
    const AVCodec* codec;
    AVCodecContext* c;
};
