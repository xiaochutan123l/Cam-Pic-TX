#include "../include/camera/cam_controller.h"
#include "../include/camera/cam_controller_impl.h"

#include "../include/communication/sender.h"
#include "../include/communication/udp_client.h"
#include <memory>

uint8_t frame_rate = 30;
std::unique_ptr<Sender> sdr = std::make_unique<Udp_client>();
struct address addr = {(char*)"127.0.0.1", 9998};
struct chunk_header_video hdr_video = {
        .format = 0,
        .frame_rate = frame_rate,
        .codec = 0,
        .reserved = 0,
        .timestamp = 0,
    };
union chunk_header hdr = {hdr_video};
//MSG_TYPE type = MSG_TYPE::VIDEO_MSG;
//MSG_FLAGS flags = MSG_FLAGS::NONE;

Stream_fmt fmt = {RESOLUTION_FMT::R_480,
                        frame_rate,
                        IMAGE_FMT::JPEG};
char *dev_name = (char*)"/dev/video0";
Cam_Controller *cam = new Cam_Controller_Impl(fmt, dev_name);
/*
frame capture handler callback function.
*/
// void handler_cb_write(void *start, uint32_t length) {
//     printf("handler callback function, frame size: %d bytes\n", length);
// 	//FILE *out_fp = fopen("./test.jpg","w");
//     FILE *out_fp = fopen(filename.c_str(), "w");
// 	if(out_fp==NULL) {
// 		printf("File cannot be opened\n");
// 		exit(EXIT_FAILURE);
// 	}
// 	else
// 	{
// 		printf("File opened for writing\n");
// 		fwrite(start, length, 1, out_fp);
// 	}
// 	fclose(out_fp);
// }

void handler_cb_send(void *start, uint32_t length) {
    printf("handler callback function, frame size: %d bytes\n", length);
    sdr->send_frame((const uint8_t *)start,
                    length,
                    0, // static_cast<uint8_t>(type), 
                    0, // static_cast<uint8_t>(flags),
                    &hdr
                    );
    printf("sent: %d bytes\n", length);
}

void init_sender() {
    sdr->connect(addr);
    printf("connected\n");
}

void init_camera() {
    std::cout << "start cam ..." << std::endl;
    cam->start(handler_cb_send);
    //cam->start(handler_cb_fake);
    std::cout << "camera ready, wait for capturing" << std::endl;
}

void deinit_camera() {
    std::cout << "capture done" << std::endl;
    cam->close();
    std::cout << "exit" << std::endl;
    delete cam;
}

int main() {
    /* set up camera */
    init_camera();
    int frame_count = 30;
    for (int i = 0; i < frame_count; i++){
        //filename = filename_base + std::to_string(i) + filename_ext;
        cam->get_frame();
    }

    /* set up sender */
    init_sender();

    printf("ready for send\n");
    //MSG_TYPE type = MSG_TYPE::VIDEO_MSG;
    
    //int len = sdr->recv_packet(recv_buf, (size_t)recv_buf_len);
    //printf("received: %s\n", recv_buf);


    return 1;
}