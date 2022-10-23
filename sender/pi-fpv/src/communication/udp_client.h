#pragma once 

#include "sender.h"

class Udp_client : public Sender {
public:
    // defualt udp.
    Udp_client();
    Udp_client(sock_type type);

    void connect(struct address &addr);

    void send_frame(const uint8_t * frame_buf, size_t frame_len) override;

    void send_frame_to(const uint8_t * frame_buf, size_t frame_len, struct address &addr) override;

    void send_packet(uint8_t * msg_buf, size_t msg_len, struct chunk_header &hdr) override;

    void void send_packet_to(uint8_t * msg_buf, size_t msg_len, struct chunk_header &hdr, struct address &addr) override;

    void reset_addr(struct address &new_addr) override;

    void reset_payload_len(size_t new_pl_len) override;

    ~Udp_client();

private:
    int sockfd;
	struct sockaddr_in m_serv, m_client;
	socklen_t m_sock_len_c;
	socklen_t m_sock_len_s;
    size_t m_chunk_pl_len;
    struct address m_addr;

    uint8_t *m_snd_buf;

    void add_dst_addr(struct address &addr);
    init_udp();

}