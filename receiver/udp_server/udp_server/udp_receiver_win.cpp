#include "udp_receiver_win.h"

UDP_Receiver::UDP_Receiver(int bufLen, int Port)
{
	snd_buf_len = bufLen;
	port = Port;
}

void UDP_Receiver::init()
{
	// Initialise Winsock.
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((servSock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = INADDR_ANY;
	sockAddr.sin_port = htons(port);

	// bind.
	if (bind(servSock, (struct sockaddr *)&servSock, sizeof(servSock)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Bind done");
}

int UDP_Receiver::send_packet(char* snd_buf, int data_len)
{
	if ((snd_len = sendto(servSock, snd_buf, data_len, 0, (struct sockaddr *) &clntAddr, clntAddr_len)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	return snd_len;
}

int UDP_Receiver::receive_packet(char* recv_buf)
{
	printf("Waiting for data...");

	//clear the buffer by filling null, it might have previously received data
	memset(recv_buf, '\0', snd_buf_len);

	//try to receive some data, this is a blocking call
	if ((recv_len = recvfrom(servSock, recv_buf, snd_buf_len, 0, (struct sockaddr *) &clntAddr, &clntAddr_len)) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	//printf("Received packet from %s:%d\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port));
	return recv_len;
}

void UDP_Receiver::close()
{
	closesocket(servSock);
	WSACleanup();
}
