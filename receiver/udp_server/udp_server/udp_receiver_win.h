#pragma once

#include <winsock2.h>
#include <stdio.h>
#include "packet_handler.h"
#include <stdint.h>
#include <vector>
#include <fstream>
#include <iostream>

#pragma comment(lib,"ws2_32.lib") //Winsock Library
#pragma warning(disable:4996)

class UDP_Receiver {
public:
	UDP_Receiver(int bufLen, int Port);
	void init();
	int send_packet(char* snd_buf, int data_len);
	int receive_packet(char* recv_buf);
	void close();

private:
	WSADATA wsa;
	SOCKET servSock;
	struct sockaddr_in sockAddr;
	// maybe change char to uin8_t, because uint8_t is exactly 8 bits, but char is at least 8 bits.
	SOCKADDR clntAddr;
	int clntAddr_len = sizeof(clntAddr);
	int snd_buf_len;
	int port;
	int snd_len;
	int recv_len;
};