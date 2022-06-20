#ifndef _PACKET_HANDLER_H_
#define _PACKET_HANDLER_H_
#include <stdint.h>
/*
UDP packet handling related. 
*/

#define HEADER_LEN 8
#define CHUNK_LEN 1400 // header not included
#define PAYLOAD_SIZE HEADER_LEN + CHUNK_LEN

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

/*
parse header from a chunk.
*/
struct chunk_header unpack_header(uint8_t * pld_buffer, uint8_t hdr_len);


/*
pack chunk header to send buffer.
*/ 
void pack_header (uint8_t * buffer, struct chunk_header &hdr);

#endif /*_PACKET_HANDLER_H_*/