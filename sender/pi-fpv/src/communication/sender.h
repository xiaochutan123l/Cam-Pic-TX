#pragma once 

#include <stddef.h>
#include "packet_handler.h"

class Sender {
public:
    Sender(){};

    virtual void connect(struct address &addr) = 0;

    virtual void send_frame(const uint8_t * frame_buf, 
                            size_t frame_len, 
                            uint8_t msg_type, 
                            uint8_t flags, 
                            struct chunk_header *c_hdr) = 0;

    virtual void send_frame_to(const uint8_t * frame_buf, 
                                size_t frame_len, 
                                uint8_t msg_type, 
                                uint8_t flags, 
                                struct chunk_header *c_hdr,
                                struct address &addr) = 0;

    //virtual void send_packet(const uint8_t * msg_buf, size_t msg_len, struct chunk_header &hdr) = 0;

    //virtual void send_packet_to(const uint8_t * msg_buf, size_t msg_len, struct chunk_header &hdr, struct address &addr) = 0;

    //https://linux.die.net/man/2/recvfrom
    virtual ssize_t recv_packet(uint8_t *recv_buf, size_t recv_buf_size) = 0;

    virtual void reset_addr(struct address &new_addr) = 0;

    virtual void reset_payload_len(size_t new_pl_len) = 0;
    
    virtual ~Sender(){};
};
