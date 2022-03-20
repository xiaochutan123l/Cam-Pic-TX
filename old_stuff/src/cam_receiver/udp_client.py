import socket
import time
addr = "127.0.0.1"
port = 9998
bufferSize  = 1024
# Create a datagram socket
udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind to address and ip
try:
    while(True):
        udp_sock.sendto("hello".encode(), (addr, port))
        #print(f"receive {len(msg)}-byte message from {client_addr}")
        print("send a packet")
        time.sleep(1)
except KeyboardInterrupt as e:
    print(e)

udp_sock.close()