#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include <string>
#include <iostream>

//#include "sender.h"
#include "packet_handler.h"

int m_sockfd;
struct sockaddr_in m_serv, m_client;
socklen_t m_sock_len_c;
socklen_t m_sock_len_s;
//uint16_t m_chunk_pl_len;
//struct address m_addr;

check_header(uint8_t *recv_buf, int recv_len); {
    std::cout << "type: " << get_msg_type(recv_buf) << std::endl;
    // TODO: print all header info.
    std::cout << "type: " << get_msg_type(recv_buf)
        << ", flags: " << hdr.flags 
        << ", total_chunk_num: " << hdr.total_chunk_num 
        << ", chunk_num: " << hdr.chunk_num
        << "chunk_len: " << hdr.chunk_len 
        << std::endl;
         
}

int main() {
    m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    m_serv.sin_family = AF_INET;
    m_serv.sin_port = htons(9998);
    m_serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    m_sock_len_c = sizeof(m_client);
    m_sock_len_s = sizeof(m_serv);

    if (bind(m_sockfd, (struct sockaddr *)&m_serv, sizeof(m_serv)) == SO_ERROR) {
        printf("Bind failed with error code\n");
        exit(EXIT_FAILURE);
    }
    
    int frame_buf_len = 30000;
    uint8_t * frame_buf = (uint8_t*)calloc(recv_buf_len, sizeof(uint8_t));

    int recv_buf_len = 2048;
    uint8_t * recv_buf = (uint8_t*)calloc(recv_buf_len, sizeof(uint8_t));
    int recv_len = 0;
	
    int total_recv_bytes = 0;

    struct packet_header pkt_hdr;

    while (1) {
        recv_len = recvfrom(m_sockfd, recv_buf, recv_buf_len, 0, (struct sockaddr*)&m_client, &m_sock_len_c); 
        //chunk_hdr = unpack_header(recv_buf, HEADER_LEN);
        check_header(recv_buf, recv_len);
        struct msg_header msg_hdr;
        unpack_msg_header(recv_buf, &msg_hdr);

        memcpy(recv_buf, frame_buf_len + total_recv_bytes, msg_hdr.chunk_len-CHUNK_HDR_LEN);

        total_recv_bytes += recv_len - PKT_HDR_LEN;

        if (msg_hdr.chunk_len != recv_len - MSG_HDR_LEN) {
            std::cout << "-----receive bytes size error -------" << stfd::endl;
        }
        if (msg_hdr.chunk_num == msg_hdr.total_chunk_num) {
            std::cout << "get full packets" << stfd::endl;
            break;
        }
    }

    std::string msg = "received successully!";
    uint16_t msg_len = sizeof(msg);
    const char *m_snd_buf = msg.c_str();
    std::cout << "msg len : " << msg_len << std::endl;

    sendto(m_sockfd, m_snd_buf, msg_len, 0, (struct sockaddr *)&m_client, m_sock_len_c);

    close(m_sockfd);
    free(frame_buf);
    free(recv_buf);
    return 1;
}