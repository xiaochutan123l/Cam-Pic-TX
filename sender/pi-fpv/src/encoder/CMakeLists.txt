set(SRC_LIST 
    encoder_impl.cpp
    )

add_library(encoder ${SRC_LIST})

# 设置包含路径
include_directories(/usr/local/include)

# 查找库文件
find_library(LIBAVCODEC avcodec /usr/local/lib)
find_library(LIBAVUTIL avutil /usr/local/lib)
find_library(LIBPTHREAD pthread)
find_library(LIBM m)
find_library(LIBX264 x264 /usr/local/lib)

# 链接库
target_link_libraries(encoder ${LIBAVCODEC} ${LIBAVUTIL} ${LIBPTHREAD} ${LIBM} ${LIBX264})