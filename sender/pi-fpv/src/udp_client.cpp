#include<iostream>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "packet_handler.h"

using namespace std;

void send_init_udp(int sockfd, struct sockaddr_in & serv, socklen_t m);
void send_frame(const uint8_t * buf,
	size_t frame_len,
	const uint16_t pld_len,
	int sockfd,
	struct sockaddr_in & serv,
	socklen_t m
);
void send_chunk(uint8_t * buffer,
	struct chunk_header &hdr,
	const uint8_t * msg_buffer,
	size_t msg_len, int sockfd,
	struct sockaddr_in & serv,
	socklen_t m
);

void error(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

int main()
{
	int sockfd;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in serv, client;

	serv.sin_family = AF_INET;
	serv.sin_port = htons(9998);
	serv.sin_addr.s_addr = inet_addr("192.168.2.104");

	//char buffer[256];
	socklen_t l = sizeof(client);
	socklen_t m = sizeof(serv);
	//socklen_t m = client;
	cout << "\ngoing to send\n";
	cout << "\npls enter the mssg to be sent\n";
	//fgets(buffer, 256, stdin);
	//sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&serv, m);
	//recvfrom(sockfd, buffer, 256, 0, (struct sockaddr *)&client, &l);


	const int data_len = 5000;
	uint8_t data[data_len];
	memset(data, 1, data_len);

	// Set chunk length 1400, max.1436. For UDP payload
	const uint16_t pld_len = PAYLOAD_SIZE;
	// Send the entire frame data. Of course it should be fragmented to fit the UDP payload size.

	send_init_udp(sockfd, serv, m);

	for (int i = 0; i < 10; i++) {
		send_frame(data, data_len, pld_len, sockfd, serv, m);
		sleep(2);
	}

}

void send_init_udp(int sockfd, struct sockaddr_in & serv, socklen_t m) {
	const uint8_t msg[] = "hello world";
	uint8_t buffer[30];

	for (uint16_t i = 0; i< 5; i++) {
		struct chunk_header hdr = { 0, 0, 5, (uint16_t)(i + 1), sizeof(msg) };
		send_chunk(buffer, hdr, msg, sizeof(msg), sockfd, serv, m);
		printf("sent udp packet");
		sleep(2);
	}
}

/*
Fragment the entire frame buffer and send them out.
TODO: pack and insert header field for each packet.
*/
void send_frame(const uint8_t * buf, 
				size_t frame_len, 
				const uint16_t pld_len, 
				int sockfd, 
				struct sockaddr_in & serv,
				socklen_t m
				) {

	uint8_t snd_buffer[pld_len];       // send buffer
									   //uint16_t snd_buf_len = sizeof(snd_buffer);   // send buffer length
	uint16_t rest = frame_len % CHUNK_LEN;       // last chunk data length
	uint16_t total_chunk_num = frame_len / CHUNK_LEN;   // total chunk number
	if (rest) {
		total_chunk_num += 1;
	}
	//uint16_t count;

	for (uint16_t i = 0; i < total_chunk_num - 1; ++i) {
		struct chunk_header hdr = { 1, 0, total_chunk_num, (uint16_t)(i + 1), CHUNK_LEN };
		send_chunk(snd_buffer, hdr, buf + (i * CHUNK_LEN), CHUNK_LEN, sockfd, serv, m);
	}

	if (rest) {
		struct chunk_header hdr = { 1, 0, total_chunk_num, total_chunk_num, rest };
		send_chunk(snd_buffer, hdr, buf + (frame_len - rest), rest, sockfd, serv, m);
	}
}

/*
insert header into the message.
*/
void send_chunk(uint8_t * buffer, 
				struct chunk_header &hdr, 
				const uint8_t * msg_buffer, 
				size_t msg_len, int sockfd, 
				struct sockaddr_in & serv,
				socklen_t m
				) {
	// Insert header to the beginning of the send buffer.
	//uint8_t * ptr = buffer;

	memcpy(buffer, &hdr, sizeof(hdr));
	// Copy the message data after the header
	memcpy(buffer + sizeof(hdr), msg_buffer, msg_len);
	sendto(sockfd, buffer, HEADER_LEN + msg_len, 0, (struct sockaddr *)&serv, m);
}
