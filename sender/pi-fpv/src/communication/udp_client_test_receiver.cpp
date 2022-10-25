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

void print(struct chunk_header &hdr) {
    std::cout << "type: " << hdr.type 
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
    
    int recv_buf_len = 2048;
    uint8_t * recv_buf = (uint8_t*)calloc(recv_buf_len, sizeof(uint8_t));
    int recv_len = 0;
	
    struct chunk_header chunk_hdr;

    for (int i = 0; i < 100; i++) {
        recv_len = recvfrom(m_sockfd, recv_buf, recv_buf_len, 0, (struct sockaddr*)&m_client, &m_sock_len_c); 
        chunk_hdr = unpack_header(recv_buf, HEADER_LEN);
        print(chunk_hdr);
    }

    std::string msg = "received successully!";
    uint16_t msg_len = sizeof(msg);
    const char *m_snd_buf = msg.c_str();
    std::cout << "msg len : " << msg_len << std::endl;

    sendto(m_sockfd, m_snd_buf, msg_len, 0, (struct sockaddr *)&m_client, m_sock_len_c);

    close(m_sockfd);
    free(recv_buf);
    return 1;
}