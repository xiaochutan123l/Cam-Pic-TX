import socket

addr = "127.0.0.1"
port = 9998
bufferSize  = 1024
# Create a datagram socket
udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind to address and ip
udp_sock.bind((addr, port))
print("UDP server up and listening")

try:
    while(True):
        data, addr = udp_sock.recvfrom(1024)
        #print(f"receive {len(msg)}-byte message from {client_addr}")
        print("received a packet")
except KeyboardInterrupt as e:
    print(e)

sock.close()