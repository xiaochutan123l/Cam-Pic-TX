#include "packet_handler.h"


void unpack_msg_header(uint8_t * pkt_buffer, struct msg_header_v2 *msg_hdr) {
    memcpy(msg_hdr, pkd_buffer, MSG_HDR_LEN);
}

void unpack_chunk_video_header(uint8_t * pkt_buffer, struct chunk_header_video_v2 *chunk_hdr) {
    memcpy(chunk_hdr, pkd_buffer, CHUNK_HDR_LEN);
}
void unpack_chunk_audio_header(uint8_t * pkt_buffer, struct chunk_header_audio_v2 *chunk_hdr) {
    memcpy(chunk_hdr, pkd_buffer, CHUNK_HDR_LEN);
}

void unpack_packet_header(uint8_t * pkt_buffer, struct packet_header_v2 *pkd_hdr) {
    memcpy(pkt_hdr, pkd_buffer, PKT_HDR_LEN);
}

uint8_t get_msg_type(uint8_t * pkt_buffer) {
    return pkd_buffer[0];
}

struct chunk_header unpack_header(uint8_t * pkd_buffer, uint8_t hdr_len){
    struct chunk_header ret_hdr;
    struct chunk_header * hdr_ptr;
    //hdr_ptr = (chunk_header *)(buffer + hdr_len);
    hdr_ptr = (chunk_header *)pkd_buffer;
    ret_hdr.type = hdr_ptr->type;
    ret_hdr.flags = hdr_ptr->flags;
    ret_hdr.total_chunk_num = hdr_ptr->total_chunk_num;
    ret_hdr.chunk_num = hdr_ptr->chunk_num;
    ret_hdr.chunk_len = hdr_ptr->chunk_len;
    return ret_hdr;
}

void to_sock_addr(struct address addr) {
    // TODO
}

void print_header(struct chunk_header &hdr) {
    // TODO
}
