set(ENCODER_TARGET encoder)

add_library(encoder encode_video.c)

set(FFMPEG_LIBS
    avcodec
    avformat
    avutil
    # avdevice
    # swscale
    # swresample
    # postproc
)

foreach(LIB ${FFMPEG_LIBS})
    find_path(${LIB}_INCLUDE_DIR lib${LIB})
    find_library(${LIB}_LIBRARY ${LIB})

    if(${LIB}_INCLUDE_DIR AND ${LIB}_LIBRARY)
        message("Found ${LIB} in ${${LIB}_INCLUDE_DIR} and ${${LIB}_LIBRARY}")
        target_include_directories(${ENCODER_TARGET} PUBLIC ${${LIB}_INCLUDE_DIR})
        target_link_libraries(${ENCODER_TARGET} PUBLIC ${${LIB}_LIBRARY})
    else()
        message(WARNING "Could not find ${LIB}")
    endif()
endforeach()

target_include_directories(encoder PUBLIC ../../include/encoder)