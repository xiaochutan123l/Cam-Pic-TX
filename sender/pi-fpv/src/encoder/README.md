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

Hardware acceleration h264-omx only supports 32bit.

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
 
