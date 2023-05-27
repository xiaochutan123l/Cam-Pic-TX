#include <iostream>
#include <vector>
#include <string>
#include <fstream>

// 将指定图像帧编码为 AVPacket
AVPacket* encodeFrame(Encoder& encoder, const AVFrame* frame) {
    return encoder.encodeFrame(const_cast<AVFrame*>(frame));
}

// 将指定 AVPacket 解码为 AVFrame
AVFrame* decodePacket(Decoder& decoder, const AVPacket* pkt) {
    return decoder.decodePacket(const_cast<AVPacket*>(pkt));
}

int main() {
    // 假设有一帧图像数据，存储在一个 uint8_t 数组中
    std::vector<uint8_t> imageData;  // 假设这里是实际的图像数据

    // 创建 AVFrame 并填充图像数据
    AVFrame* frame = av_frame_alloc();
    frame->width = 1920;  // 图像宽度
    frame->height = 1080; // 图像高度
    frame->format = AV_PIX_FMT_RGB24; // 图像像素格式
    av_frame_get_buffer(frame, 0);
    memcpy(frame->data[0], imageData.data(), imageData.size());

    // 创建编码器
    Encoder encoder("h264", frame->width, frame->height, {1, 25}, 400000);

    // 编码图像帧
    AVPacket* encodedPacket = encodeFrame(encoder, frame);

    // 创建解码器
    Decoder decoder("h264");

    // 解码数据包
    AVFrame* decodedFrame = decodePacket(decoder, encodedPacket);

    // 将解码后的图像数据保存为文件
    std::ofstream outputFile("decoded_frame.pgm", std::ios::binary);
    outputFile << "P5\n" << decodedFrame->width << " " << decodedFrame->height << "\n255\n";
    for (int y = 0; y < decodedFrame->height; ++y) {
        outputFile.write(reinterpret_cast<const char*>(decodedFrame->data[0] + y * decodedFrame->linesize[0]), decodedFrame->width);
    }
    outputFile.close();

    // 释放资源
    av_packet_free(&encodedPacket);
    av_frame_free(&frame);
    av_frame_free(&decodedFrame);

    return 0;
}
