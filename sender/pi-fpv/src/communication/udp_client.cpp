#include<iostream>
#include <cstring>

#include <cstdio>
#include "udp_client.h"

using namespace std;

Udp_client::Udp_client() {
	init_udp();
}

Udp_client::Udp_client(sock_type type) {
	switch(type) {
		case sock_type::UDP:
			init_udp();
			break;
	}
}

void Udp_client::connect(struct address &addr) {
	add_dst_addr(addr);
}

void Udp_client::init_udp() {
	m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	m_serv.sin_family = AF_INET;
	m_sock_len_c = sizeof(m_client);
	m_sock_len_s = sizeof(m_serv);

	//init send buffer.
	m_chunk_pl_len = PAYLOAD_LEN;
	// TODO: maybe replace recv_buf_len with a more suitable value.
	m_snd_buf = (uint8_t*)calloc(RECV_BUF_LEN, sizeof(uint8_t));
}

void Udp_client::add_dst_addr(struct address &addr) {
	// TODO: add = operator overload function.
	m_addr.ip = addr.ip;
	m_addr.port = addr.port;
	m_serv.sin_port = htons(addr.port);
	m_serv.sin_addr.s_addr = inet_addr(addr.ip);
}

// TODO: add new send frame function with customized header information.!!!
// TODO: 1. change to new packet header
//		 2. try to use some more effective method to handle the packet pack and unpack.
void Udp_client::send_frame(const uint8_t * frame_buf, size_t frame_len) {    
	uint16_t rest = (uint16_t)(frame_len % m_chunk_pl_len);       // last chunk data length
	uint16_t total_chunk_num = (uint16_t)(frame_len / m_chunk_pl_len);   // total chunk number
	if (rest) {
		total_chunk_num += 1;
	}
	// TODO: replace i* m_chunk_pl_len with a counter to record the buf offset.
	for (uint16_t i = 0; i < total_chunk_num - 1; ++i) {
		struct chunk_header hdr = { 1, 0, total_chunk_num, (uint16_t)(i + 1), m_chunk_pl_len};
		send_packet(frame_buf + (i * m_chunk_pl_len), m_chunk_pl_len, hdr);
	}

	if (rest) {
		struct chunk_header hdr = { 1, 0, total_chunk_num, total_chunk_num, rest};
		send_packet(frame_buf + (frame_len - rest), rest, hdr);
	}
}

void Udp_client::send_frame_to(const uint8_t * frame_buf, size_t frame_len, struct address &addr) {
	add_dst_addr(addr);
	send_frame(frame_buf, frame_len);
}

void Udp_client::send_packet(const uint8_t * msg_buf, size_t msg_len, struct chunk_header &hdr) {
	// Insert header to the beginning of the send buffer.
	//printf("send_packet\n");
	memcpy(m_snd_buf, &hdr, HEADER_LEN);
	//printf("copy1\n");
	// Copy the message data after the header
	memcpy(m_snd_buf + HEADER_LEN, msg_buf, msg_len);
	//printf("copy2\n");
	sendto(m_sockfd, m_snd_buf, HEADER_LEN + msg_len, 0, (struct sockaddr *)&m_serv, m_sock_len_s);
}

void Udp_client::send_packet_to(const uint8_t * msg_buf, size_t msg_len, struct chunk_header &hdr, struct address &addr) {
	add_dst_addr(addr);
	send_packet(msg_buf, msg_len, hdr);
}

// block receive.
ssize_t Udp_client::recv_packet(uint8_t *recv_buf, size_t recv_buf_size) {
	//int flag = MSG_WAITALL;
	return recvfrom(m_sockfd, recv_buf, recv_buf_size, 0, (struct sockaddr*)&m_client, &m_sock_len_c); 
}

void Udp_client::reset_addr(struct address &new_addr) {
	add_dst_addr(new_addr);
}

void Udp_client::reset_payload_len(size_t new_pl_len) {
	free(m_snd_buf);
	m_chunk_pl_len = new_pl_len;
	m_snd_buf = (uint8_t*)calloc(new_pl_len, sizeof(uint8_t));
}

Udp_client::~Udp_client() {
	free(m_snd_buf);
    close(m_sockfd);
}
