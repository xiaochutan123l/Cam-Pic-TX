### commend.

target = udp_sender

cc = g++

objects = ./src/udp_client.cpp packet_handler.o v4l2_capture.o

$(target) : $(objects)
	$(cc) -o $(target) $(objects)

packet_handler.o : ./src/packet_handler.cpp
	$(cc) -c ./src/packet_handler.cpp

v4l2_capture.o : ./src/v4l2_capture.cpp
	$(cc) -c ./src/v4l2_capture.cpp

clean:
	rm ./packet_handler.o ./v4l2_capture.o $(target)
