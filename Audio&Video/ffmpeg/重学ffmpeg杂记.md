[TOC]

# 概要

本篇记录一些ffmpeg的杂项。

## 本机设备的一些命令

使用ffmpeg命令找出机器上的所有采集设备：

```c
// 首先找到机器上的设备列表
➜  ~ ffmpeg -devices
ffmpeg version N-82679-gc9dcd25 Copyright (c) 2000-2016 the FFmpeg developers
  built with Apple LLVM version 8.0.0 (clang-800.0.42.1)
  configuration:
  libavutil      55. 41.101 / 55. 41.101
  libavcodec     57. 66.108 / 57. 66.108
  libavformat    57. 58.101 / 57. 58.101
  libavdevice    57.  2.100 / 57.  2.100
  libavfilter     6. 67.100 /  6. 67.100
  libswscale      4.  3.101 /  4.  3.101
  libswresample   2.  4.100 /  2.  4.100
Devices:
 D. = Demuxing supported
 .E = Muxing supported
 --
 D  avfoundation    AVFoundation input device
 D  lavfi           Libavfilter virtual input device

 // 然后找出机器上的采集设备
➜  ~ ffmpeg -f avfoundation -list_devices true -i ""
ffmpeg version N-82679-gc9dcd25 Copyright (c) 2000-2016 the FFmpeg developers
  built with Apple LLVM version 8.0.0 (clang-800.0.42.1)
  configuration:
  libavutil      55. 41.101 / 55. 41.101
  libavcodec     57. 66.108 / 57. 66.108
  libavformat    57. 58.101 / 57. 58.101
  libavdevice    57.  2.100 / 57.  2.100
  libavfilter     6. 67.100 /  6. 67.100
  libswscale      4.  3.101 /  4.  3.101
  libswresample   2.  4.100 /  2.  4.100
[AVFoundation input device @ 0x7ff875c01d20] AVFoundation video devices:
[AVFoundation input device @ 0x7ff875c01d20] [0] FaceTime HD Camera
[AVFoundation input device @ 0x7ff875c01d20] [1] Capture screen 0
[AVFoundation input device @ 0x7ff875c01d20] AVFoundation audio devices:
[AVFoundation input device @ 0x7ff875c01d20] [0] Built-in Microphone
[AVFoundation input device @ 0x7ff875c01d20] [1] DongleAudio录音
```

在FFmpeg中采集OS X系统的输入输出设备，常规方式采用的是OS X的avfoundation设备进行采集。

Windows采集设备的主要方式是dshow、vfwcap、gdigrab，其中dshow可以用来抓取摄像头、采集卡、麦克风等，vfwcap主要用来采集摄像头类设备，gdigrab则是抓取Windows窗口程序。

## 接口使用

### 使用avformat_open_input打开Mac上的摄像头碰到的问题

在Xcode中新建了一个Command Line Tools的C语言工程，结果下面的代码执行的时候报一个日志：

```c
    const char *camera_name = "FaceTime HD Camera";
    AVDictionary *fmt_opt = NULL;
    av_dict_set_int(&fmt_opt, "framerate", 30, 0);
    
    int ret = avformat_open_input(&input_fmt_ctx, camera_name, input_fmt, &fmt_opt);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, av_err2str(ret));
        exit(1);
    }
```

输出的时候报告错误如下，其中有两个问题，一个是pixel format的问题，另一个是crash的问题。

#### 首先看像素格式的问题

```c
[avfoundation @ 0x105002000] Selected pixel format (yuv420p) is not supported by the input device.
[avfoundation @ 0x105002000] Supported pixel formats:
[avfoundation @ 0x105002000]   uyvy422
[avfoundation @ 0x105002000]   yuyv422
[avfoundation @ 0x105002000]   nv12
[avfoundation @ 0x105002000]   0rgb
[avfoundation @ 0x105002000]   bgr0
[avfoundation @ 0x105002000] Overriding selected pixel format to use uyvy422 instead.
2019-08-03 17:05:54.255123+0800 ffmpeg_practice[2814:45297] [access] This app has crashed because it attempted to access privacy-sensitive data without a usage description.  The app's Info.plist must contain an NSCameraUsageDescription key with a string value explaining to the user how the app uses this data.
Program ended with exit code: 9
```

其中那句"Selected pixel format (yuv420p) is not supported by the input device."我以为是avformat_open_input的参数哪里传的有问题，结果试了以下ffmpeg的命令：

```c
ffmpeg -f avfoundation -framerate 30 -i "1:0" ~/Downloads/out.mkv
```

输出结果也是有这么一句: "Selected pixel format (yuv420p) is not supported by the input device."，如下：

```c
[avfoundation @ 0x7fa9a2004800] Selected pixel format (yuv420p) is not supported by the input device.
[avfoundation @ 0x7fa9a2004800] Supported pixel formats:
[avfoundation @ 0x7fa9a2004800]   uyvy422
[avfoundation @ 0x7fa9a2004800]   yuyv422
[avfoundation @ 0x7fa9a2004800]   nv12
[avfoundation @ 0x7fa9a2004800]   0rgb
[avfoundation @ 0x7fa9a2004800]   bgr0
[avfoundation @ 0x7fa9a2004800] Overriding selected pixel format to use uyvy422 instead.
[avfoundation @ 0x7fa9a2004800] Stream #0: not enough frames to estimate rate; consider increasing probesize
```

看来这只是程序运行中的警告之类的。

#### crash的问题

至于crash的问题，因为macOS 10.14之后使用设备需要申请权限，所以这里的意思应该是说Xcode不具备使用摄像头的权限，因为我是使用Xcode启动的程序。找到Xcode生成的可执行文件地址，直接在终端中执行的是就可以正确打开摄像头了。

# Xcode中对ffmpeg进行调试

[TOC]

# ffmpeg的编译

Mac上，可以使用homebrew来安装ffmpeg，默认会安装在/usr/local/Cellar/ffmpeg下。

如果这个想要在Xcode中调试ffmpeg相关的代码，比如为了可以在调试的时候可以step into找到ffmpeg的source code，最好是自己编译一下生成一个*可玩的，可修改*的库。而homebrew下载的库让它在原地呆着。

为了让生成的ffmpeg放在自定义的目录。在编译ffmpeg的时候，需要配置一下默认的安装路径设置prefix，如下：
设定本篇ffmpeg的路径位于/Users/hj/ffmpeg
```
./configure --prefix=/Users/hj/ffmpeg/build --enable-shared
```

上面的命令还添加了编译项 --enable-shared。因为默认情况下，ffmpeg的./configure只配置生成静态库(\*.a)，添加了这一项，才会生成动态库(\*.dylib)。设置好之后，开始经典流程：

```
make
make install
```

执行结束之后，编译生成的代码就在指定位置（/Users/hj/ffmpeg/build）了。

# Xcode的配置

举个例子，新建一个Command Line工程。

在Xcode中通过右键添加文件的方式。将ffmpeg整个工程的代码，注意只需要使用文件夹引用即可。嫌麻烦的话，这一步不用也可以，不过添加进来就是为了查看代码方便。

设置头文件和库的搜索路径:
1. 选中新建的工程，
2. 选择**PROJECT**中的项，在Build Settings中找到并设置**Header Search Paths**为：/Users/hj/ffmpeg/include；设置**Library Search Paths**为：/Users/hj/ffmpeg/static。
3. 选择**TARGETS**中的项，在Build Settings中找到相应的项，做上述设置。
4. 选择**TARGETS**中的项，在Gerneral中，找到**Linked Frameworks and Libraries**添加相应的库和framework，ffmpeg的静态库是要添加的，除此之外，根据不同的使用情况，还有一些系统的framework需要添加，可以根据报错信息提示来添加，这里先给一个列表：

- CoreVideo.framework
- CoreMedia.framework
- AppKit.framework
- Security.framework
- VideoToolbox.framework
- AudioToolbox.framework
- libiconv.tbd
- liblzma.tbd
- libbz2.tbd
- libz.tbd

配置好之后就可以 在新建的工程中调试ffmpeg的代码了。

## Xcode中的调试问题

因为Xcode一旦在Libraries Search Path中找到了*.dylib，就会尝试优先加载它。所以生成的代码最好不要将静态库(\*.a)和动态库(\*.dylib)放在一起。而是采用这样的方式：

```
/Users/hj/ffmpeg/build/static/*.a
/Users/hj/ffmpeg/build/dylibs/*.dylib
```

参考了: [Xcode linking against static and dynamic library](https://stackoverflow.com/questions/45699389/xcode-linking-against-static-and-dynamic-library?rq=1)


上面的步骤需要手动操作，新建两个目录static、dylibs，然后移动相应的文件到文件夹中。
目前我的调试都是使用**静态库**的，使用动态库没有找到使用Xcode调试进ffmpeg代码的方法。