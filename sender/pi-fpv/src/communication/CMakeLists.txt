set(SRC_LIST 
    udp_client.cpp
    packet_handler.cpp
    )

if (NOT TARGET communication)
add_library(communication ${SRC_LIST})
endif()    


target_include_directories(communication PUBLIC ../../include/communication)