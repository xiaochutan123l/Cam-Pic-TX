#pragma once 

#include "packet_handler.h"

class Sender {
public:
    Sender();
    Sender(struct address addr);
    Sender(struct address addr, sock_type type);

    virtual void init() = 0;
    virtual void connect(string server_ip, int server_port) = 0;

    virtual void send_frame(const uint8_t * buf, size_t frame_len) = 0;

    virtual void send_frame_to(const uint8_t * buf, size_t frame_len, struct address addr) = 0;

    virtual void send_chunk(uint8_t * buffer, struct chunk_header &hdr) = 0;

    virtual void send_chunk_to(uint8_t * buffer, struct chunk_header &hdr, struct address addr) = 0;

    virtual void reset_addr(struct address new_addr) = 0;
    // TODO: add asychronous receive and send mechanism.
    //virtual void register_recv_callback() = 0;


    virtual ~Sender(){};
}