#include<iostream>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "packet_handler.h"
#include <cstdio>
#include "udp_client.h"

using namespace std;

/*
 这个方案暂时不考虑使用真异步UDP，因为图传中，发送者主要以采集摄像头数据并且发送给接收者，而接收者主要是接受数据。
 同时，考虑到一般的图传模块和控制模块是分开的，如果不是特别极限的应用场景，这种模式有点浪费资源，因为需要使用两个无线模块。
 本方案接受和发送都使用同一个无线模块，同一个socket实体来接受和发送数据，在一般场景下是这样。万一需要用在特殊场景下，
 也可以只保留本方案中的单一方向传输的功能，然后增加单独的控制模块，丝毫不影响兼容性。

 由于UDP是无连接协议，没有像TCP那样的流控制，拥塞控制和重传机制，所以很灵活，本方案也是看中的这个特性，
 但是众所周知现实中的网络并不是完美的，丢包是无法避免的，所以在UDP的基础上，针对图传加控制应用场景增加了acknowledge机制。
 但是又不像TCP的acknowledge机制那么高密度，以一个数据包为单位，本方案以一帧图像或者一个h264/h265数据帧为单位，一般来说，一帧图像数据小则几十k，大则几百甚至以M为单位，
 当开始接收到下一帧图像数据的数据包时，接收端给发送端发送反馈数据包。
 如果只是反馈简单的成功收到一帧图像这种信息，专门用一个数据包有点浪费，所以本方案复用这个acknowledgement数据包，包含一下信息并发送给发送端：
  1. 接收情况 - 一帧成功/失败
  2. 网络状况 - 用 一帧图像中 接收失败/总数据包量 来计算当前网络状况比如丢包率等。
			   如果网络状况差，也可以进行自动降低帧率清晰度等操作
			   network coding编码会带来一定程度的延时，但是在网络状况差的时候是值得的。
  3. 网络数据指令： 如果网络状况差，那么就开启network coding机制，由于是用于图传的方案，所以不考虑重传，某些帧丢失就丢失，毕竟发送端可以同时在本地储存，视频数据最终是完好保存下来的。
  4. 控制指令： 这里用来发送发送端的小车，无人机或其他载有相机的装置进行控制的信息，由于这种控制信息一般都很短，所以放置在这里很合适。

特殊情况：
- 如果视频/图像一帧太大，那么acknowlegement数据包将以小于一帧图像的间隔进行。 发送反馈数据包间隔 = min（指定数量数据包， 一帧图像数据所需数据包数量）
- 由于网络存在丢包，而控制等信息很重要，必须保证送到。所以为了保证反馈数据包送达，采用两个机制同时保证接受端能收到：
	1. 每次连续发送2-3次，由于采集两帧图像之间有间隔，所以发送两帧数据也会有间隔，接受端接收两帧数据之间也会有间隔，所以多发两次不会有延时上的影响。
	2. 接受端发送端图像数据包包头内记录控制信息的接收情况，对每个有效指令数据包（重复的只算一个）进行编号，如果接收到，那么就记录上一个收到的编号，放进包头发送出去
	   接受端收到就知道需不需要重新发送。

由于两帧之间时间间隔的存在，还有时间可以让发送端发送出了图像数据意外的数据，比如更新发送端设备，比如摄像头和传感器等信息发送给接受端。

针对以上机制，将设计专用的数据传输协议，包括数据包头以及在代码中各个机制的实现。

当然，wifi+udp只是一种传输方案，虽然不考虑TCP-like的协议，但是如同openhd使用的braodcast方式也将会进行实现，由于代码低耦合，不同传输方案和不同设备都可以方便灵活切换。
*/
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
	m_snd_buf = (uint8_t*)calloc(CHUNK_SIZE, sizeof(uint8_t));
}

void Udp_client::add_dst_addr(struct address &addr) {
	// TODO: add = operator overload function.
	m_addr.ip = addr.ip;
	m_addr.port = addr.port;
	m_serv.sin_port = htons(addr.port);
	m_serv.sin_addr.s_addr = inet_addr(addr.ip);
}

// TODO: add new send frame function with customized header information.!!!
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
	memcpy(m_snd_buf, &hdr, sizeof(hdr));
	// Copy the message data after the header
	memcpy(m_snd_buf + sizeof(hdr), msg_buf, msg_len);
	sendto(m_sockfd, m_snd_buf, HEADER_LEN + msg_len, 0, (struct sockaddr *)&m_serv, m_sock_len_s);
}

void Udp_client::send_packet_to(const uint8_t * msg_buf, size_t msg_len, struct chunk_header &hdr, struct address &addr) {
	add_dst_addr(addr);
	send_packet(msg_buf, msg_len, hdr);
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
