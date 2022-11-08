#ifndef _PACKET_HANDLER_H_
#define _PACKET_HANDLER_H_
#include <stdint.h>
/*
UDP packet handling related.
*/

#define HEADER_LEN 8
#define PAYLOAD_LEN 1400 // header not included
#define CHUNK_SIZE HEADER_LEN + PAYLOAD_LEN
#define RECV_BUF_LEN 2048

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
- Chunk Length: Total length of this packet/chunk (header included)
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
	- Chunk Length: the whole packet length include header
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

/*
parse header from a chunk.
*/
struct chunk_header unpack_header(uint8_t * pld_buffer, uint8_t hdr_len);


/*
pack chunk header to send buffer.
*/
void pack_header(uint8_t * buffer, struct chunk_header &hdr);

struct address {
	char *ip;
	int port;
};

void to_sock_addr(struct address addr);

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

#endif /*_PACKET_HANDLER_H_*/
