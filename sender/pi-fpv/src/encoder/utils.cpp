#include <string>
#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavutil/mem.h>

}

bool yuyvToJpg(const std::string& inputFilename, const std::string& outputFilename, int width, int height) {
    // 初始化 FFmpeg 库
    //avcodec_register_all();

    // 创建 AVCodecContext 和 AVFrame
    //AVCodec* codec = avcodec_find_encoder_by_name(AV_CODEC_ID_MJPEG);
    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);


    if (!codec) {
        std::cerr << "Codec not found" << std::endl;
        return false;
    }

    AVCodecContext* codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        std::cerr << "Failed to allocate codec context" << std::endl;
        return false;
    }

    codecContext->width = width;
    codecContext->height = height;
    codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    codecContext->codec_type = AVMEDIA_TYPE_VIDEO;

    /* 打开编码器之后 */

    codecContext->time_base = (AVRational){1, 30};
    codecContext->strict_std_compliance = FF_COMPLIANCE_UNOFFICIAL;


/* 设置其他编码器参数 */


    // 打开编码器
    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        std::cerr << "Failed to open codec" << std::endl;
        return false;
    }

    AVFrame* frame = av_frame_alloc();
    if (!frame) {
        std::cerr << "Failed to allocate frame" << std::endl;
        return false;
    }

    // 分配输入缓冲区
    int buffer_size = av_image_get_buffer_size(codecContext->pix_fmt, width, height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(buffer_size);
    av_image_fill_arrays(frame->data, frame->linesize, buffer, codecContext->pix_fmt, width, height, 1);

    // 设置 AVFrame 的数据
    // 这里假设输入文件是 YUYV 格式的原始数据，你需要根据实际情况进行修改
    FILE* inputFile = fopen(inputFilename.c_str(), "rb");
    if (!inputFile) {
        std::cerr << "Failed to open input file" << std::endl;
        return false;
    }
    fread(frame->data[0], 1, width * height * 2, inputFile);
    fclose(inputFile);

    // 创建 AVPacket 来保存编码后的 JPEG 数据
    AVPacket* packet = av_packet_alloc();
    if (!packet) {
        std::cerr << "Failed to allocate packet" << std::endl;
        return false;
    }

    // 发送帧给编码器并接收编码后的数据
    if (avcodec_send_frame(codecContext, frame) < 0) {
        std::cerr << "Failed to send frame to encoder" << std::endl;
        return false;
    }

    if (avcodec_receive_packet(codecContext, packet) < 0) {
        std::cerr << "Failed to receive packet from encoder" << std::endl;
        return false;
    }

    // 将编码后的数据写入文件
    FILE* outputFile = fopen(outputFilename.c_str(), "wb");
    if (!outputFile) {
        std::cerr << "Failed to open output file" << std::endl;
        return false;
    }
    fwrite(packet->data, 1, packet->size, outputFile);
    fclose(outputFile);

    // 释放资源
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codecContext);
    av_free(buffer);

    return true;
}

int main() {
    yuyvToJpg("output-7200.yuv", "output-7200.jgp", 1280, 720);
    return 0;
}
