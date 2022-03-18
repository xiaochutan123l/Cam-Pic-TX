#ifndef _PACKET_HANDLER_H_
#define _PACKET_HANDLER_H_

/*
UDP packet handling related. 
*/

/**
 * Packet header for each UDP packet.
 */
struct chunk_header {
	uint8_t type;
	uint8_t flags;
	uint16_t total_chunk_num,
    uint16_t chunk_num,
    uint16_t chunk_len, 
};

chunk_header * extract_header(uint8_t * buffer);

//pack_header ();

#endif /*_PACKET_HANDLER_H_*/