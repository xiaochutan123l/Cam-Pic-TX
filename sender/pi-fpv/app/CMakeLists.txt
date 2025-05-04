#add_subdirectory(../src)

# 在 app/CMakeLists.txt 文件中添加以下内容
set(SRC_BINARY_DIR ${CMAKE_BINARY_DIR}/src_binary)  # 定义一个二进制目录
file(MAKE_DIRECTORY ${SRC_BINARY_DIR})              # 创建二进制目录

add_library(qrcode_reader_lib SHARED IMPORTED)
set_target_properties(qrcode_reader_lib PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libZXing.so.2.3.0
    #INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include  # 改成你头文件的路径
)

# 将 src 目录作为子目录添加到二进制目录
add_subdirectory(../src ${SRC_BINARY_DIR})

# add_executable(fpv_sender fpv_sender.cpp)
# target_include_directories(fpv_sender PUBLIC ../include)
# target_link_libraries(fpv_sender camera communication encoder)

add_executable(qrcode_scanner QRCodeScanner.cpp)

target_include_directories(qrcode_scanner PUBLIC ../include)

target_link_libraries(qrcode_scanner camera communication qrcode_reader_lib)

# 设置运行时可以在当前目录找到 .so（否则运行时报错找不到库）
set_target_properties(qrcode_scanner PROPERTIES
    BUILD_RPATH "$ORIGIN"
)