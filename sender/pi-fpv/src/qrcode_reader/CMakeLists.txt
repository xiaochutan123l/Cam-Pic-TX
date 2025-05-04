add_library(qrcode_reader_lib SHARED IMPORTED)
message(STATUS "QRCodeReader lib: ${CMAKE_CURRENT_SOURCE_DIR}/libZXing.so.2.3.0")
# 指定编译好的 so 路径（相对路径或绝对路径均可）
set_target_properties(qrcode_reader_lib PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libZXing.so.2.3.0
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
)