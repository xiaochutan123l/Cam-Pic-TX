### commend.

target = udp_sender
dir = ./src
cc = g++

build_dir = ./build

objects = $(dir)/udp_client.cpp packet_handler.o v4l2_capture.o
offline_test_objects = $(dir)/stream_cap_test.cpp $(dir)/cam_controller_impl.cpp $(dir)/v4l2_capture.c
#test_objects = $(dir)/stream_cap_test.cpp packet_handler.o v4l2_capture.o

$(target) : $(objects)
	$(cc) -o $(target) $(objects)

offline_test: $(offline_test_objects)
	$(cc) -o $(build_dir)/offline_test $(offline_test_objects)

clean:
	rm $(build_dir)/*

#test: 

	
