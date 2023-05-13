set(SRC_LIST 
    udp_client.cpp
    packet_handler.cpp
    )

add_library(communication ${SRC_LIST})

target_include_directories(communication PUBLIC ../../include/communication)