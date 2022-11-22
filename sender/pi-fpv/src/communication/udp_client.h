#pragma once 

#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>

#include "sender.h"
#include "packet_handler.h"

class Udp_client : public Sender {
public:
    // defualt udp.
    Udp_client();
    Udp_client(sock_type type);

    void connect(struct address &addr);

    void send_frame(const uint8_t * frame_buf, 
                    size_t frame_len, 
                    uint8_t msg_type, 
                    uint8_t flags, 
                    struct chunk_header *c_hdr) override;

    void send_frame_to(const uint8_t * frame_buf, 
                        size_t frame_len, 
                        size_t frame_len, 
                        uint8_t msg_type, 
                        uint8_t flags, 
                        struct chunk_header *c_hdr, 
                        struct address &addr) override;

    //void send_ack(const uint8_t * ack_buf, size_t ack_len) override;

    ssize_t recv_packet(uint8_t *recv_buf, size_t recv_buf_size);

    void reset_addr(struct address &new_addr) override;

    void reset_payload_len(size_t new_pl_len) override;

    ~Udp_client();

private:
    void send_packet(const uint8_t * msg_buf, size_t msg_len, struct packet_header *pkt_hdr);

    //void send_packet_to(const uint8_t * msg_buf, size_t msg_len, struct packet_header *pkt_hdr, struct address &addr);

    int m_sockfd;
    // TODO: change client to vector if later multiple server/client should communicate.
	struct sockaddr_in m_serv, m_client;
    // store the source address length of the incoming packet.
	socklen_t m_sock_len_c;
	socklen_t m_sock_len_s;
    uint16_t m_chunk_pl_len;
    struct address m_addr;
    uint8_t *m_snd_buf;

    void add_dst_addr(struct address &addr);
    void init_udp();

};
