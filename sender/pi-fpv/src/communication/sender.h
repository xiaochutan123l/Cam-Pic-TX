#pragma once 

#include "packet_handler.h"

class Sender {
public:
    Sender();

    virtual void connect(struct address &addr) = 0;

    virtual void send_frame(const uint8_t * frame_buf, size_t frame_len) = 0;

    virtual void send_frame_to(const uint8_t * frame_buf, size_t frame_len, struct address &addr) = 0;

    virtual void send_packet(uint8_t * msg_buf, size_t msg_len, struct chunk_header &hdr) = 0;

    virtual void send_packet_to(uint8_t * msg_buf, size_t msg_len, struct chunk_header &hdr, struct address &addr) = 0;

    virtual void reset_addr(struct address &new_addr) = 0;

    virtual void reset_payload_len(size_t new_pl_len) = 0;
    
    virtual ~Sender(){};
}