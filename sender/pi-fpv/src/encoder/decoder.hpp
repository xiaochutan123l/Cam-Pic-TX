
class Decoder {
public:
    Decoder(const std::string& codecName) {
        codec = avcodec_find_decoder_by_name(codecName.c_str());
        if (!codec) {
            std::cerr << "Codec '" << codecName << "' not found" << std::endl;
            exit(1);
        }

        c = avcodec_alloc_context3(codec);
        if (!c) {
            std::cerr << "Could not allocate video codec context" << std::endl;
            exit(1);
        }

        if (avcodec_open2(c, codec, NULL) < 0) {
            std::cerr << "Could not open codec" << std::endl;
            exit(1);
        }
    }

    ~Decoder() {
        avcodec_free_context(&c);
    }

    AVFrame* decodePacket(AVPacket* pkt) {
        int ret = avcodec_send_packet(c, pkt);
        if (ret < 0) {
            std::cerr << "Error sending a packet for decoding" << std::endl;
            exit(1);
        }

        AVFrame* frame = av_frame_alloc();
        if (!frame) {
            std::cerr << "Could not allocate video frame" << std::endl;
            exit(1);
        }

        ret = avcodec_receive_frame(c, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return nullptr;
        else if (ret < 0) {
            std::cerr << "Error during decoding" << std::endl;
            exit(1);
        }

        return frame;
    }

    // Setters and getters
    void setCodecContext(AVCodecContext* context) {
        avcodec_parameters_to_context(c, context->par_in);
    }

    AVCodecContext* getCodecContext() const {
        return c;
    }

private:
    const AVCodec* codec;
    AVCodecContext* c;
};
