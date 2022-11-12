#include "packet_handler.h"


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

void pack_header(uint8_t * buffer, struct chunk_header &hdr){
    struct chunk_header * hdr_ptr;
    hdr_ptr = (chunk_header *)buffer;
    hdr_ptr->type = hdr.type;
    hdr_ptr->flags = hdr.flags;
    hdr_ptr->total_chunk_num = hdr.total_chunk_num;
    hdr_ptr->chunk_num = hdr.chunk_num;
    hdr_ptr->chunk_len = hdr.chunk_len;
}

void to_sock_addr(struct address addr) {
    // TODO
}

void print_header(struct chunk_header &hdr) {
    // TODO
}
