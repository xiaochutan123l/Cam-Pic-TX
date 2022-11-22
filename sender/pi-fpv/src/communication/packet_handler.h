#ifndef _PACKET_HANDLER_H_
#define _PACKET_HANDLER_H_
#include <stdint.h>
/*
UDP packet handling related.
*/

// deprecated.
#define HEADER_LEN 8

#define PAYLOAD_LEN 1400 // header not included
#define CHUNK_SIZE HEADER_LEN + PAYLOAD_LEN
#define RECV_BUF_LEN 2048

#define PKT_HDR_LEN 20
#define PKT_HDR_LEN_ACK 12
#define MSG_HDR_LEN 12
#define CHUNK_HDR_LEN 8


// deprecated
/**
* Packet header for each UDP packet.
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|      Type     |     Flags     |      Total Chunk Number       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         Chunk Number          |         Chunk Length          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

*/
struct chunk_header {
	uint8_t type;
	uint8_t flags;
	uint16_t total_chunk_num;
	uint16_t chunk_num;
	uint16_t chunk_len;
};

/* Messager Header*/
/*
Message structure:

+-----------------------+
|   Message Header      |
+-----------------------+
|   Chunk Header        |
+-----------------------+
|   Data                |
+-----------------------+

A message consist of Message header, a chunk header(only for audio/video message) and its data.

Message Header

0               1               2               3

0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+---------------+---------------+------------------------------+
|  Message Type |     Flag      |        Chunk Number          |
+---------------+---------------+------------------------------+
|      Total Chunk Number       |        Chunk Length          |
+-------------------------------+------------------------------+
|                       Frame Sequence Number                  |
+--------------------------------------------------------------+



- Message Type:
	- Video: This message data is a video frame
	- Audio: This message data is a audio frame
	- Ack: This message is a pure ack for the last frame received by receiver
	- Ack+request: This message contains a normal ack and some bunded requests(control message) 
- Flag:
- Chunk Number: count the current chunk number of this message
- Total Chunk Number: total number of chunks in this message
- Chunk Length: Total length of this chunk (chunk header included, but message header not)
- Frame Sequence Number: individual sequence number of each message(frame), audio and video are separatly

Chunk：Header - Video

+---------------+---------------+---------------+--------------+
|     Format    |  Frame Rate   |     Codec     |   Reserved   |
+---------------+---------------+---------------+--------------+
|                           Timestamp                          |
+--------------------------------------------------------------+

- Format: Video frame format
- Frame Rate: Frame rate
- Codec: encoder name
- Reserved: for potential useage
- Timestamp: timestamp of this frame

Chunk Header - Audio

+---------------+---------------+---------------+--------------+
|     Format    |Channel Number |  Sample Rate  |    Depth     |
+---------------+---------------+---------------+--------------+
|                           Timestamp                          |
+--------------------------------------------------------------+


- Format: Audio frame format
- Channel Number: channel number of this audio frame
- Sample Rate: audio frame sample rate [Hz]
- Depth: sample depth [bit]
- Timestamp: timestamp of this frame

ACK/ACK_CTL Header

0               1               2               3

0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+---------------+---------------+---------------+--------------+
|  Message Type |   Flag        |  Loss Rate    | Chunk Number |
+---------------+---------------+---------------+--------------+
|     Frame Sequence Number     |         Frame ACK            |
+-------------------------------+------------------------------+
|          Chunk Length         |        Request Number        |
+-------------------------------+------------------------------+

ACK data:

+-------------------------------+------------------------------+
|         Request Type 1        |        Request Data 1        |
+-------------------------------+------------------------------+
|                               .                              |
|                               .                              |
|                               .                              |
+-------------------------------+------------------------------+
|         Request Type N        |        Request Data N        |
+-------------------------------+------------------------------+

An ACK packet will be sent by receiver after receiving each audio or video frame,
to tell sender some network conditions
Meanwhile some request for control message can be added to this kind of message for efficiency.
It will be sufficient for requests which are not latency sensetive.
If realtime requierd, then set flag to urgen, or use separat wireless module for this kind of realtime message.

- Header: 
	- Message Type: ACK/ACK_Request
	- Flag: this ack sent for an audio or video frame
	- Loss Rate: record the packet loss rate [n of 100, or % number] of last frame
	- Chunk Number: in order to avoid packet loss because of UDP, multiple ack packet will
		        be transmitted repeatly (3 suggested), udp server can drop the repeated
			packet if already received one
	- Frame Sequence Number: cooresponding frame sequence number of this frame
	- Frame ACK: acknowlegement, report if the last frame recievd successfully
	- Chunk Length: sounds confused, but chunk length here means the whole chunk data length not include header
	- Request Number: the number of bunded requests

- Data: A serials of requests.
*/

enum class sock_type {
	UDP,
	TCP,
	RAW_UDP,
	RAW_TCP,
	DPDK,
};

// 8 Byte
struct chunk_header_video {
	uint8_t format;
	uint8_t frame_rate;
	uint8_t codec;
	uint8_t reserved;
	uint32_t timestamp;
};

// 8 byte
struct chunk_header_audio {
	uint8_t format;
	uint8_t frame_rate;
	uint8_t codec;
	uint8_t reserved;
	uint32_t timestamp;
};

union chunk_header {
	struct chunk_header_video hdr_v;
	struct chunk_header_audio hdr_a;
};

struct msg_header{
	uint8_t msg_type;
	uint8_t flags;
	uint16_t chunk_num;
	uint16_t total_chunk_num;
	uint16_t chunk_len;
	uint32_t frame_seq_num;
};

// 20 Byte
struct packet_header{
	struct msg_header msg_header;
	union chunk_header chunk_header;
};

// 12 Byte
struct ack_packet_header {
	uint8_t msg_type;
	uint8_t flags;
	uint8_t loss_rate;
	uint8_t chunk_num;
	uint16_t frame_seq_num;
	uint16_t frame_ack;
	uint16_t chunk_len;
	uint16_t request_num;	
};

enum class MSG_TYPE {
	VIDEO_MSG,
	AUDIO_MSG,
	ACK,
	ACK_REQ,
};

// not defined yet
enum class MSG_FLAGS {
	// =1, 2, 4, 8...
	NONE;
}

enum class V_FMT {
	RGB24,
	JPEG,
	YUV420;
	// need add more
}

enum class A_FMT {
	PCM,
	// need more
}

enum class ACK_FLAGS {
	AUDIO,
	VIDEO,
}

enum class ACK {
	// 100% received
	SUCCESSFUL,
	// loss can be covered by padding
	ACCEPTABLE,
	// loss can not be covered.
	FIALED,
}

enum class ACK_REQ {
	// add network coding
	ADD_NC,
	// need more
}

// TODO: maybe set buffer pointer to data position and return a uint8_t *ptr.


void unpack_msg_header(uint8_t * pkt_buffer, struct msg_header *msg_hdr);
void unpack_chunk_video_header(uint8_t * pkt_buffer, struct chunk_header_video *chunk_hdr);
void unpack_chunk_audio_header(uint8_t * pkt_buffer, struct chunk_header_audio *chunk_hdr);
void unpack_packet_header(uint8_t * pkt_buffer, struct packet_header *pkd_hdr);

uint8_t get_msg_type(uint8_t * pkt_buffer);

void pack_packet_header(uint8_t * pkt_buffer, struct packet_header &hdr);

/*
parse header from a chunk.
*/
struct chunk_header unpack_header(uint8_t * pkt_buffer, uint8_t hdr_len);

struct address {
	char *ip;
	int port;
};

void to_sock_addr(struct address addr);

#endif /*_PACKET_HANDLER_H_*/
