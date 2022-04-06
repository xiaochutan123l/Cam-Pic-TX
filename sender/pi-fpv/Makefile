### commend.

target = udp_sender

cc = g++

objects = ./src/udp_client.cpp packet_handler.o

$(target) : $(objects)
	$(cc) -o $(target) $(objects)

packet_handler.o : ./src/packet_handler.cpp
	$(cc) -c ./src/packet_handler.cpp


clean:
	rm ./packet_handler.o $(target)
