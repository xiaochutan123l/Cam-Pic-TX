#include <stdio.h>
#include "udp_client.h"
#include <memory>

int main() {
    printf("start\n");
    
    std::unique_ptr<Sender> sdr = std::make_unique<Udp_client>();
    
    struct address addr = {(char*)"127.0.0.1", 9998};
    sdr->connect(addr);
    printf("connected\n");

    uint16_t msg_len = 1400;
    uint8_t * msg_buf = (uint8_t*)calloc(msg_len, sizeof(uint8_t));
    int recv_buf_len = 2048;
    uint8_t * recv_buf = (uint8_t*)calloc(recv_buf_len, sizeof(uint8_t));

    printf("ready for send\n");

    struct chunk_header hdr_muster = {0, 0, 100, 0, msg_len};
    for (uint16_t i = 0; i < 100; i++) {
        //printf("sending %d-th packet\n", i);
        hdr_muster.chunk_num = i;
        //printf("sending %d-th packet2\n", i);
        sdr->send_packet(msg_buf, msg_len, hdr_muster);
        printf("sent %d-th packet\n", i);
    }

    int len = sdr->recv_packet(recv_buf, (size_t)recv_buf_len);
    printf("received: %s\n", recv_buf);
    
    return 1;
}