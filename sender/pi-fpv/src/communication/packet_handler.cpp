#include "../../include/communication/packet_handler.h"

std::ostream& operator << (std::ostream& o, const chunk_header_video& hdr) {
    return std::cout << "Video chunk header: " 
                     << "format: " << (int)hdr.format
                     << ", frame_rate: " << (int)hdr.frame_rate
                     << ", codec: " << (int)hdr.codec
                     << ", reserved: " << (int)hdr.reserved
                     << ", timestamp: " << hdr.timestamp
                     << std::endl; 
}

std::ostream& operator << (std::ostream& o, const chunk_header_audio& hdr) {
    return std::cout << "Audio chunk header: " 
                     << "format: " << (int)hdr.format
                     << ", chuannel number: " << (int)hdr.channel_num
                     << ", sample rate: " << (int)hdr.sample_rate
                     << ", depth: " << (int)hdr.depth
                     << ", timestamp: " << hdr.timestamp
                     << std::endl; 
}

std::ostream& operator << (std::ostream& o, const msg_header& hdr) {
    return std::cout << "Message header: " 
                     << "message type: " << (int)hdr.msg_type
                     << ", flags: " << (int)hdr.flags
                     << ", chunk number: " << hdr.chunk_num
                     << ", total chunk number: " << hdr.total_chunk_num
                     << ", chunk length: " << hdr.chunk_len
                     << ", frame sequence number: " << hdr.frame_seq_num
                     << std::endl; 
}

std::ostream& operator << (std::ostream& o, const packet_header& hdr) {
    MSG_TYPE t = static_cast<MSG_TYPE>(hdr.msg_header.msg_type);
    switch (t) {
        case MSG_TYPE::VIDEO_MSG:
            return std::cout << hdr.msg_header << hdr.chunk_header.hdr_v << std::endl;
        case MSG_TYPE::AUDIO_MSG:
            return std::cout << hdr.msg_header << hdr.chunk_header.hdr_a << std::endl;
        default:
            // TODO: add ack stuff later
            return std::cout << "wrong message type" << std::endl;
    }
}

void unpack_msg_header(uint8_t * pkt_buffer, struct msg_header *msg_hdr) {
    memcpy(msg_hdr, pkt_buffer, MSG_HDR_LEN);
}

void unpack_chunk_video_header(uint8_t * pkt_buffer, struct chunk_header_video *chunk_hdr) {
    memcpy(chunk_hdr, pkt_buffer, CHUNK_HDR_LEN);
}
void unpack_chunk_audio_header(uint8_t * pkt_buffer, struct chunk_header_audio *chunk_hdr) {
    memcpy(chunk_hdr, pkt_buffer, CHUNK_HDR_LEN);
}

void unpack_packet_header(uint8_t * pkt_buffer, struct packet_header *pkt_hdr) {
    memcpy(pkt_hdr, pkt_buffer, PKT_HDR_LEN);
}

uint8_t get_msg_type(uint8_t * pkt_buffer) {
    return pkt_buffer[0];
}

/*
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
*/
void to_sock_addr(struct address addr) {
    // TODO
}

/*
void print_header(struct chunk_header &hdr) {
    // TODO
}
*/