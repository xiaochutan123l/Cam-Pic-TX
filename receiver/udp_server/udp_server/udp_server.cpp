/*
Simple UDP Server
*/

#include <stdio.h> // c printf
#include <winsock2.h>
#include "packet_handler.h"
#include <stdint.h>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;
// #include <iostream.h> c++ , std::cout
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#pragma warning(disable:4996)

//void save_data(uint8_t* jpgData, size_t len);

#define BUFLEN 2048	//Max length of buffer
#define PORT 9998	//The port on which to listen for incoming data

int total_len = 0;
int total_packet = 0;

int main()
{
	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	// maybe change char to uin8_t, because uint8_t is exactly 8 bits, but char is at least 8 bits.
	char buf[BUFLEN];
	//uint8_t buf[BUFLEN];
	WSADATA wsa;

	slen = sizeof(si_other);

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	// byte_vector store the receiving udp bytes.
	std::vector<uint8_t> byte_vector;

	//keep listening for data
	while (1)
	{
		printf("Waiting for data...");
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		//print details of the client/peer and the data received
		//printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		//printf("Data: %s\n", buf);
		total_packet += 1;
		//total_len += sizeof(buf);
		printf("recv %d udp packets and %d bytes\n", total_packet, recv_len);

		struct chunk_header hdr = unpack_header((uint8_t *)buf, HEADER_LEN);
		printf("header: type %d, flags %d, total_chunk_num %d, chunk_num %d, chunk_len %d\n", hdr.type, 
											hdr.flags, hdr.total_chunk_num, hdr.chunk_num, hdr.chunk_len);
		if (hdr.type == 0) {
			printf("Data: %s\n", buf + HEADER_LEN);
		}

		if (hdr.type == 1) {
			std::vector<uint8_t> temp_vector((uint8_t *)buf + HEADER_LEN, (uint8_t *)buf + recv_len);
			printf("sizeof tmp vector %d \n", temp_vector.size());
			byte_vector.insert(byte_vector.end(), temp_vector.begin(), temp_vector.end());
			if (hdr.chunk_num == hdr.total_chunk_num) {
				// received all chunks for a frame.
				//byte_vector.clear();
				break;
			}
		}
	
		//now reply the client with the same data
		//if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
		//{
		//	printf("sendto() failed with error code : %d", WSAGetLastError());
		//	exit(EXIT_FAILURE);
		//}
	}

	// already get a frame.
	printf("received %d bytes frame", byte_vector.size());
	//uint8_t jpgData[byte_vector.size()] = 
	//uint8_t* jpgData = byte_vector.data();
	//save_data(jpgData, byte_vector.size());
	//byte_vector.data();
	// drawJPG().

	ofstream outfile;
	outfile.open("frame_num.jpg", 'wb');
	printf("sizeof vector %d \n", byte_vector.size());
	//for (uint8_t byte : byte_vector) {
		//std::cout << byte << " ";
	//	printf("%d ", (uint8_t) byte);
	//}
	/*
	for (uint8_t byte : byte_vector) {
		//std::cout << byte << " ";
		printf("byte: %d", byte);
		outfile << (uint8_t) byte;
	}*/
	uint8_t * jpgData = byte_vector.data();
	size_t len = byte_vector.size();
	/*
	for (int i = 0; i < len; i++) {
		//std::cout << byte << " ";
		//printf("byte: %d", byte);
		outfile << *jpgData;
		jpgData++;
	}*/
	FILE *fp = fopen("frame_num.jpg", "wb");
	fwrite(jpgData, sizeof(uint8_t), byte_vector.size(), fp);
	fclose(fp);
	// 向文件写入用户输入的数据
	//outfile << jpgData << endl;
	// 关闭打开的文件
	outfile.close();

	getchar();

	closesocket(s);
	WSACleanup();

	return 0;
}

