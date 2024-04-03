#pragma once

class EncoderImpl {
public:
    EncoderImpl();
    ~EncoderImpl();
    void init();

    int receive_packet(void *packet_buffer, size_t &size);
    void send_frame(const uint8_t *frame_buffer, size_t size);
private:
    void prepare_encoder();
    void set_format();
    void open_encoder();
    void open_in_file();
    void open_out_file();
    void start_encode();
    void close_encoder();

    const AVCodec *_codec;
    AVCodecContext *_enc_ctx= NULL;
    AVFrame *_frame;
    AVPacket *_pkt;

    //const char *filename;
    const char *_codec_name;

    //FILE *out_file;
    //const std::string& in_file_name;

    size_t _frame_pts;
};