#!/bin/sh
PREFIX=/usr/local
./configure \
--enable-gpl    --enable-version3 --enable-nonfree \
--enable-static --disable-shared \
--cpu=armv7-a \
--target-os=linux \
--prefix=$PREFIX \
\
--disable-everything \
\
--enable-small \
\
--enable-ffmpeg \
--enable-ffprobe \
\
--enable-libx264 \
--enable-encoder=libx264 \
--enable-decoder=h264 \
--enable-encoder=aac \
--enable-decoder=aac \
--enable-encoder=ac3 \
--enable-decoder=ac3 \
--enable-encoder=rawvideo \
--enable-decoder=rawvideo \
--enable-encoder=mjpeg \
--enable-decoder=mjpeg \
\
--enable-demuxer=concat \
--enable-muxer=flv \
--enable-demuxer=flv \
--enable-demuxer=live_flv \
--enable-muxer=hls \
--enable-muxer=segment \
--enable-muxer=stream_segment \
--enable-muxer=mov \
--enable-demuxer=mov \
--enable-muxer=mp4 \
--enable-muxer=mpegts \
--enable-demuxer=mpegts \
--enable-demuxer=mpegvideo \
--enable-muxer=matroska \
--enable-demuxer=matroska \
--enable-muxer=wav \
--enable-demuxer=wav \
--enable-muxer=pcm* \
--enable-demuxer=pcm* \
--enable-muxer=rawvideo \
--enable-demuxer=rawvideo \
--enable-muxer=rtsp \
--enable-demuxer=rtsp \
--enable-muxer=rtsp \
--enable-demuxer=sdp \
--enable-muxer=fifo \
--enable-muxer=tee \
\
--enable-parser=h264 \
--enable-parser=aac \
\
--enable-protocol=tcp \
\
--enable-filter=aresample \
--enable-filter=allyuv \
--enable-filter=scale \
--enable-libfreetype \
\
--enable-indev=v4l2 \
--enable-indev=alsa \
\
--enable-outdev=v4l2 \
--enable-outdev=alsa \
\
--enable-v4l2-m2m \
--enable-encoder=h264_v4l2m2m \
--enable-decoder=h264_v4l2m2m \
--enable-hwaccel=v4l2_m2m \
\
#--enable-mmal \
#--enable-omx \
#--enable-omx-rpi \
#\
#--enable-hwaccel=h264_mmal \
#--enable-decoder=h264_mmal \
#--enable-encoder=h264_omx \
\