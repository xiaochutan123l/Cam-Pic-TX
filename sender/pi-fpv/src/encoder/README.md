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
h264-omx is partially deprecated by raspberry pi.
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
 
 ### 可能的问题：

 v4l2m2m 不支持b帧，并且没有gop参数。
 如果之后换成其他设备或者系统需要考虑重新测试是否支持。
 HEVC->H.265
Failed to set gop size: Invalid argument: 但是并没有解决我的问题。
https://www.mail-archive.com/ffmpeg-user@ffmpeg.org/msg28288.html

- 树莓派zero2w上支持h264硬件加速，但是gop和max_b_frames 参数ioctl不支持，应该是
  树莓派硬件或者驱动的问题。但是libx264这些参数都是支持的，没有问题
  v4l2m2m源码：
  [FFmpeg/libavcodec/v4l2_m2m_enc.c](https://github.com/FFmpeg/FFmpeg/blob/master/libavcodec/v4l2_m2m_enc.c)

- 鉴于此问题，可以测试v4l2m2m和libx264哪个效率更好，毕竟树莓派不是最终使用的硬件
  等换硬件之后测试是否支持这些参数设置。如果支持h265/HEVC更好

- libavcodec 可以设置zerocopy低延时模式：
    [从ffmpeg源代码分析如何解决ffmpeg编码的延迟问题](https://zhuanlan.zhihu.com/p/438062997)
    ```c
    if (codec->id == AV_CODEC_ID_H264)
        //av_opt_set(c->priv_data, "preset", "slow", 0);
        // slow 模式比较慢，对于图传低延时模式更合适。
        av_opt_set(c->priv_data, "preset", "superfast", 0);
        av_opt_set(c->priv_data, "tune", "zerolatency", 0);
    ```
