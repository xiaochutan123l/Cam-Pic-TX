add_subdirectory(../src)

#add_subdirectory(communication)

#add_subdirectory(encoder)

add_executable(fpv_sender fpv_sender.cpp)

target_include_directories(fpv_sender PUBLIC ../include)

target_link_libraries(fpv_sender camera communication encoder)