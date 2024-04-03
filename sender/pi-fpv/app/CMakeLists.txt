#add_subdirectory(../src)

# 在 app/CMakeLists.txt 文件中添加以下内容
set(SRC_BINARY_DIR ${CMAKE_BINARY_DIR}/src_binary)  # 定义一个二进制目录
file(MAKE_DIRECTORY ${SRC_BINARY_DIR})              # 创建二进制目录

# 将 src 目录作为子目录添加到二进制目录
add_subdirectory(../src ${SRC_BINARY_DIR})

add_executable(fpv_sender fpv_sender.cpp)

target_include_directories(fpv_sender PUBLIC ../include)

target_link_libraries(fpv_sender camera communication encoder)