# Install ffmpeg and its dependencies.

### Environment.
pi@raspberrypi:~/encoder_bins/x264 $ lsb_release  -a
No LSB modules are available.
Distributor ID: Raspbian
Description:    Raspbian GNU/Linux 11 (bullseye)
Release:        11
Codename:       bullseye

### Remove old ffmpeg
-sudo apt-get --purge remove ffmpeg 
-sudo apt-get --purge autoremove

## Install x264 encoder.

link: https://code.videolan.org/videolan/x264/

- git clone http://git.videolan.org/git/x264.git
- cd x264
- git checkout stable
- ./configure --host=arm-unknown-linux-gnueabi --enable-static --disable-opencl
- make -j4
- sudo make install

### Install FFmpeg.

#### Check CPU arch.
-lscpu
Architecture:                    armv7l
Model name:                      Cortex-A53
-march=armv7-a

#### Install 
-git clone git://source.ffmpeg.org/ffmpeg --depth=1
-cd ffmpeg
- cp ../../Cam-Pic-TX/sender/pi-fpv/src/encoder/config_ffmpeg_rpi.sh ./
-chmod +x config_ffmpeg_rpi.sh
- ./configure --arch=armv7-a --target-os=linux --enable-gpl --enable-libx264 --enable-nonfree --enable-mmal
- make
- make install
h264-omx only supports 32bit.


