# Install ffmpeg and its dependencies.

## Prepare 

### Environment.

``` bash
pi@raspberrypi:~/encoder_bins/x264 $ lsb_release  -a
No LSB modules are available.
Distributor ID: Raspbian
Description:    Raspbian GNU/Linux 11 (bullseye)
Release:        11
Codename:       bullseye
```

### Remove old ffmpeg

```bash
sudo apt-get --purge remove ffmpeg 
sudo apt-get --purge autoremove
```

### Install dependencies:

install the following dependencies if errors:

#### omx dependencies:

```bash
sudo apt-get install libomxil-bellagio-dev
```

if still not work: update raspberry pi firmware

```bash
sudo rpi-update
sudo reboot
```

#### alsa dependency:
```bash
sudo apt-get install libasound2-dev
```

#### ERROR: --enable-hwaccel=h264_mmal did not match anything

这个无所谓，默认就是开启h264硬件加速的，不用担心，可以忽略

#### or install them at the beginning:

```bash
sudo apt-get install libasound2-dev libomxil-bellagio-dev
```

these may also help for possible errors:

```bash
sudo apt-get install libopus-dev libvpx-dev
```


## Install x264 encoder.
[x264 link](https://code.videolan.org/videolan/x264/)

``` bash
git clone http://git.videolan.org/git/x264.git
cd x264
git checkout stable
./configure --host=arm-unknown-linux-gnueabi --enable-static --disable-opencl
make -j4
sudo make install
```

### Install FFmpeg.

#### Check CPU arch.

``` bash
lscpu
Architecture:                    armv7l
Model name:                      Cortex-A53
```

raspberry pi zero2: -march=armv7-a

#### Install 

``` bash
git clone git://source.ffmpeg.org/ffmpeg --depth=1
cd ffmpeg
```
change `config_ffmpeg_rpi.sh` to remove unneccessary things, add `--disable-everything` and then `enable-xxx` what required.

``` bash
cp ../../Cam-Pic-TX/sender/pi-fpv/src/encoder/config_ffmpeg_rpi.sh ./
chmod +x config_ffmpeg_rpi.sh
./config_ffmpeg_rpi.sh   
make
sudo make install
```

其中enable-omx和enable-omx-rpi就是开启树莓派硬解码的选项
Hardware acceleration h264-omx only supports 32bit.

h264_v4l2m2m may cause green problem. 

#### configure parameters:
https://blog.csdn.net/yzhang6_10/article/details/77104211

### lib API:
https://libav.org/documentation/doxygen/master/index.html

### Links
 
#### encode video expamle
https://zhuanlan.zhihu.com/p/547340382

#### encode latency
https://www.twblogs.net/a/5e504e63bd9eee21167d3a83 

#### compile application
the libs are `/usr/local/lib`
    include `/usr/local/include`
to check the dependencies for each libs:

`pkg-config --libs libavcodec`


or complie like this:

`gcc main.c ´pkg-config --libs libavcodec´`
 
