set(SRC_LIST 
    udp_client.cpp
    )

#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

link_libraries(packet_handler.cpp)

add_library(sender ${SRC_LIST})