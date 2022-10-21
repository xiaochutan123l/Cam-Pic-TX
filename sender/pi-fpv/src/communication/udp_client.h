#pragma once 

#include "sender.h"

class Udp_client : public Sender {
public:
    Sender();
    Sender(struct address addr);
    Sender(struct address addr, sock_type type);

    void init() override;
    void connect(string server_ip, int server_port) override;

    void send_frame(const uint8_t * buf, size_t frame_len) override;

    void send_frame_to(const uint8_t * buf, size_t frame_len, struct address addr) override;

    void send_chunk(uint8_t * buffer, struct chunk_header &hdr) override;

    void send_chunk_to(uint8_t * buffer, struct chunk_header &hdr, struct address addr) override;

    void reset_addr(struct address new_addr) override;

    // TODO: add asychronous receive and send mechanism.
    //virtual void register_recv_callback() = 0;


    ~Udp_client();

private:
    int sockfd;
	struct sockaddr_in serv, client;
	socklen_t l_client;
	socklen_t l_serv;

}