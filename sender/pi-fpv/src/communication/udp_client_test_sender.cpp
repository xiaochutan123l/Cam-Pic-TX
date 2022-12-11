#include <stdio.h>
#include "udp_client.h"
#include <memory>

int main() {
    printf("start\n");
    
    std::unique_ptr<Sender> sdr = std::make_unique<Udp_client>();
    
    struct address addr = {(char*)"127.0.0.1", 9998};
    sdr->connect(addr);
    printf("connected\n");
    // TODO: use real frame data to verify received correctly.
    uint16_t frame_len = 30000;
    uint8_t * frame_buf = (uint8_t*)calloc(frame_len, sizeof(uint8_t));
    int recv_buf_len = 2048;
    uint8_t * recv_buf = (uint8_t*)calloc(recv_buf_len, sizeof(uint8_t));

    printf("ready for send\n");

    union chunk_header hdr_muster;
    hdr_muster.hdr_v = {0, 30, 1, 0, 1234};
    //MSG_TYPE type = MSG_TYPE::VIDEO_MSG;
    MSG_TYPE type = MSG_TYPE::AUDIO_MSG;
    MSG_FLAGS flags = MSG_FLAGS::NONE;
    sdr->send_frame(frame_buf,
                    frame_len,
                    static_cast<uint8_t>(type), 
                    static_cast<uint8_t>(flags),
                    &hdr_muster
                    );

    int len = sdr->recv_packet(recv_buf, (size_t)recv_buf_len);
    printf("received: %s\n", recv_buf);

    free(frame_buf);
    free(recv_buf);
    return 1;
}