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
#include <opencv2\opencv.hpp>
#include <time.h>
#include <opencv2\core\utils\logger.hpp>
#include "udp_receiver_win.h"

using namespace cv;

#define BUFLEN 2048	//Max length of buffer
#define PORT 9998	//The port on which to listen for incoming data

//using namespace std;
// #include <iostream.h> c++ , std::cout


//void save_data(uint8_t* jpgData, size_t len);
void show_Image(Mat &rawData, std::vector<uint8_t> & byte_vector);
void saveImage(std::vector<uint8_t> & byte_vector);

#define BUFLEN 2048	//Max length of buffer
#define PORT 9998	//The port on which to listen for incoming data

int total_len = 0;
int total_packet = 0;

int main()
{
	setLogLevel(utils::logging::LOG_LEVEL_SILENT);

	// utils::logging::setLogLevel(utils::logging::LOG_LEVEL_SILENT);
	//utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);

	// byte_vector store the receiving udp bytes.
	std::vector<uint8_t> byte_vector;
	Mat rawData;
	//keep listening for data
	clock_t frame_st;
	clock_t start, end;
	bool begin_frame = true;

	char buf[BUFLEN];
	//int buf_len = sizeof(buf);
	UDP_Receiver udp_receiver(BUFLEN, PORT);
	udp_receiver.init();
	int recv_len;
	struct chunk_header hdr;
	while (1)
	{
		fflush(stdout);
		total_packet += 1;
		//total_len += sizeof(buf);

		recv_len = udp_receiver.receive_packet(buf);

		hdr = unpack_header((uint8_t *)buf, HEADER_LEN);
		if (hdr.type == 0) {
			printf("recv %d udp packets and %d bytes\n", total_packet, recv_len);
			printf("header: type %d, flags %d, total_chunk_num %d, chunk_num %d, chunk_len %d\n", hdr.type,
				hdr.flags, hdr.total_chunk_num, hdr.chunk_num, hdr.chunk_len);
			printf("Data: %s\n", buf + HEADER_LEN);
		}

		if (hdr.type == 1) {
			if (begin_frame) {
				frame_st = clock();
				begin_frame = false;
			}
			//printf("header: type %d, flags %d, total_chunk_num %d, chunk_num %d, chunk_len %d\n", hdr.type,
			//	hdr.flags, hdr.total_chunk_num, hdr.chunk_num, hdr.chunk_len);
			
			std::vector<uint8_t> temp_vector((uint8_t *)buf + HEADER_LEN, (uint8_t *)buf + recv_len);
			//printf("sizeof tmp vector %zd \n", temp_vector.size());
			byte_vector.insert(byte_vector.end(), temp_vector.begin(), temp_vector.end());
			if (hdr.chunk_num == hdr.total_chunk_num) {
				// received all chunks for a frame.
				//clock_t start, end;
				//start = clock();
				printf("frame receiving time: %f", (double)(clock() - frame_st)*1000 / CLOCKS_PER_SEC);
				start = clock();
				show_Image(rawData, byte_vector);
				byte_vector.clear();
				end = clock();
				printf(",  total frame time: %f\n", (double)(end-start)*1000/CLOCKS_PER_SEC);
				//break;
				begin_frame = true;
			}
		}
	}
	//printf("received %zd bytes frame", byte_vector.size());
	//printf("sizeof vector %zd \n", byte_vector.size());
	getchar();
	udp_receiver.close();
	return 0;
}


void show_Image(Mat &rawData, std::vector<uint8_t> & byte_vector) {
	uint8_t * jpgData = byte_vector.data();
	//size_t len = byte_vector.size();
	size_t len = byte_vector.size();
	//Mat rawData(1, len, CV_8UC1, (void*)jpgData);
	rawData = Mat(1, len, CV_8UC1, (void*)jpgData);
	Mat decodedImage = imdecode(rawData, IMREAD_UNCHANGED);
	if (decodedImage.data == NULL)
	{
		printf("Error reading raw image data");
	}
	else {
		imshow("esp_image", decodedImage);
		waitKey(1);
	}
}

void saveImage(std::vector<uint8_t> & byte_vector) {
	uint8_t * jpgData = byte_vector.data();

	FILE *fp = fopen("frame_num2.jpg", "wb");
	fwrite(jpgData, sizeof(uint8_t), byte_vector.size(), fp);
	fclose(fp);
}
