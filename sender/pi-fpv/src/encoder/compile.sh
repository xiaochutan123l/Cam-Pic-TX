#!/bin/bash
gcc encode_video.c -I/usr/local/include -L/usr/local/lib -lavcodec -lavutil -lpthread -lm -lx264
