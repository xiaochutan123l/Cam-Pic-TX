#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include "AsyncUDP.h"
#include "img_converters.h"
#include "packet_handler.h"
//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM

#include "camera_pins.h"

// WiFi ssid and password
const char* ssid = "WLAN-886864";
const char* password = "17781912408437034468";
// UDP server address and port
const IPAddress udpAddress = IPAddress(192,168,2,104);
const int udpPort = 9998;

uint16_t chunk_id = 0;

void init_camera();
int start_wifi();
camera_fb_t * get_frame();
int init_async_udp();
void udp_packet_handler(AsyncUDPPacket packet);
void send_frame(const uint8_t * buf, size_t len, const uint16_t chunk_len);
void send_init_udp();
void send_chunk(uint8_t * buffer, struct chunk_header &hdr, const uint8_t * msg_buffer, size_t msg_len);

// AsyncUDP is implemented using FreeRTOS, thus it can concurrently receive and send packet.
AsyncUDP udp;

void setup() {
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  Serial.println("Start init camera...");
  init_camera();
  Serial.println("Start init wifi...");
  start_wifi();
  Serial.println("Start init udp...");
  init_async_udp();
  Serial.println("Start init udp 2...");
  send_init_udp();
  Serial.println("Start get frame...");

  // Get a frame from camera
  camera_fb_t * fb = get_frame();
  Serial.println("got camera frame");
  // Get the data buffer
  const uint8_t *data = fb->buf;
  // for test.
  /*
  uint8_t t_data[5000];
  for (int i = 0; i < 5000; i++){
    t_data[i] = (uint8_t)(i % 200);
  }
 
  const uint8_t *text_data = t_data;
  */
  // Set chunk length 1400, max.1436. For UDP payload
  const uint16_t pld_len = PAYLOAD_SIZE;
  //TODO: pack header for each udp payload.
  //pack_header()
  Serial.println("send to server");
  // Send the entire frame data. Of course it should be fragmented to fit the UDP payload size.
  send_frame(data, fb->len, pld_len);
  //send_frame(text_data, 5000, pld_len);
  Serial.println("already send");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10000);
}

void init_camera(){

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  #if defined(CAMERA_MODEL_ESP_EYE)
    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
  #endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

  #if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
  #endif
}


/*
Connect to WiFi.
*/
int start_wifi(){
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  Serial.println("\r\nWiFi connected!!!\r\nlocalIP: " + (WiFi.localIP().toString())); //打印本地IP
  return 1;
}


/*
Get a frame from camera, and return the data buffer pointer.
*/
camera_fb_t * get_frame(){
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
        ESP_LOGE(TAG, "Frame buffer could not be acquired");
        Serial.println(ESP_FAIL);
    }

  Serial.println(fb->width);
  Serial.println(fb->height);
  Serial.println(fb->format);
  Serial.println(fb->len);
  return fb;
}

/*
Set the UDP server IP and port.
AsyncUDP can receive packet without blocking the program,
the UDP packet handler method is binded by udp.onPacket(),
once a UDP packet is comming, the udp_packet_handler() will be called.
*/
int init_async_udp(){
  if (udp.connect(udpAddress, udpPort)) { 
    //TODO: send a few packets first for the connection with server.
    Serial.println("UDP connected");                
    udp.onPacket([](AsyncUDPPacket packet) { udp_packet_handler(packet); });
  }
  return 1;
}


/*
Say hello to server. 
Insure that the UDP communication is available.
TODO: communicate with UDP Server interactively similar to TCP handshake.
*/
void send_init_udp(){
  const uint8_t msg[] = "hello world";
  uint8_t buffer[30];
  
  for (uint16_t i = 0; i< 5; i++){
    struct chunk_header hdr = {0, 0, 5, (uint16_t)(i+1), sizeof(msg)};
    send_chunk(buffer, hdr, msg, sizeof(msg));
    Serial.println("sent udp packet");
    delay(2000);
  }
  Serial.println("wait wifi");
  delay(1000);
  Serial.println("start sending udp");
}


/*
UDP packet handler.
TODO: parse the incomming packet and extract the control message from Server.
      configure parameters on demand.
*/
void udp_packet_handler(AsyncUDPPacket packet){
  // handle incomming udp packets.
  Serial.println("received sth.");
}


/*
Fragment the entire frame buffer and send them out.
TODO: pack and insert header field for each packet.
*/
void send_frame(const uint8_t * buf, size_t frame_len, const uint16_t pld_len){

  uint8_t snd_buffer[pld_len];       // send buffer
  //uint16_t snd_buf_len = sizeof(snd_buffer);   // send buffer length
  uint16_t rest = frame_len % CHUNK_LEN;       // last chunk data length
  uint16_t total_chunk_num = frame_len / CHUNK_LEN;   // total chunk number
  if (rest){
    total_chunk_num += 1;
  }
  //uint16_t count;

  for (uint16_t i = 0; i < total_chunk_num - 1; ++i) {
    struct chunk_header hdr = {1, 0, total_chunk_num, (uint16_t)(i+1), CHUNK_LEN};
    send_chunk(snd_buffer, hdr, buf + (i * CHUNK_LEN), CHUNK_LEN);
   //memcpy(snd_buffer, buf + (i * buf_len), buf_len);
    //udp.write(snd_buffer, chunk_len);
  }

  if (rest) {
    struct chunk_header hdr = {1, 0, total_chunk_num, total_chunk_num, rest};
    send_chunk(snd_buffer, hdr, buf + (frame_len - rest), rest);
    //memcpy(snd_buffer, buf + (frame_len - rest), rest);
    //udp.write(snd_buffer, rest);
  }
}

/*
insert header into the message.
*/
void send_chunk(uint8_t * buffer, struct chunk_header &hdr, const uint8_t * msg_buffer, size_t msg_len){
  // Insert header to the beginning of the send buffer.
  //uint8_t * ptr = buffer;
  
  memcpy(buffer, &hdr, sizeof(hdr));
  // Copy the message data after the header
  memcpy(buffer + sizeof(hdr), msg_buffer, msg_len);
  udp.write(buffer, HEADER_LEN + msg_len);  
  /*
  Serial.printf("hdr size: %d, msg_len: %d, total len: %d\n", sizeof(hdr), msg_len, HEADER_LEN + msg_len);
  for (int i = 0; i < sizeof(hdr)+msg_len; i++){
    Serial.printf("%d ", ptr[0]);
    ptr++;
  }
  Serial.println(" \n\n");
  delay(10);*/
}