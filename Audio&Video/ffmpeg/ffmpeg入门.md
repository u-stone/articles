[TOC]

# ffmpeg中的主要模块

ffmpeg中的主要模块介绍

## AVFormat

实现了目前多媒体领域中的绝大多数**媒体封装格式**和**网络协议封装格式**。
查看源码就可以发现这里面的文件基本是以音视频编解码格式命名的，比如libavformat目录下包含了:

- h264dec.c
- mp3dec.c
- mp3enc.c
- rm.h/rm.c

还有网络协议:

- http.h/http.c
- rtmp.h ……
- rtp.h/rtp.c
- hlsproto.c hlsplaylist.h/.c hlsenc.c hls.c

这个模块中的接口一般以avformat_开头。

## AVCodec

实现了目前多媒体领域绝大多数常用的编解码格式，这个能力是通过扩展使用（MPEG4、AAC、MJPEG等自带的媒体编解码格式）第三方的编解码器，但是需要使用第三发库比如：

- 支持H.264编码需要x264编码器
- 支持MP3编码需要使用libmp3lame编码器
- ……

需要在编译ffmpeg的时候添加编译选项。当然添加的越多，编译得到的ffmpeg体积就越大。具体可以通过

```bash
./configure -help
```

来查看编译选项。
查看源码可以发现文件命名也是以音视频编解码格式成组命名的：

- mpegevide_enc.c
- mpegvideo_motion.c
- mpegvideo_parser.c
- mpegvideo_xvmc.c
- mpegvideo.h / mpegvideo.c
- mpegvideodata.h / mpegvideodata.c
- mpegvideodsp.h / mpegvideodsp.c
- mpegvideoencdsp.h / mpegvideoencdsp.c

这个模块中的接口一般以avcodec_开头。

## AVFilter

提供了一个通用的 **音频、视频、字幕等** 滤镜处理框架。filtergraph是一种包含了很多已连接的滤镜（filter）的有向图，每对滤镜之间都可以有多个连接，这个和微软公司的DirectShow处理功能模块化的方式是类似的。举个例子，我们从摄像头中采集出图像，输出给Filter，然后Filter会处理这个数据，最后输出处理好的数据。

从输入端来说，滤镜主要有三种类型：Source Filter、Sink Filter、Filter，其中Source Filter是指没有输入端的滤镜，Sink Filter是指没有输出端的滤镜，剩下的就是传输中间状态的Filter，既有输入端又有输出端。

FFmpeg中预置了很多滤镜，这些滤镜在avfilter中主要分为三种类型的滤镜：音频滤镜、视频滤镜、多媒体滤镜。

这个模块中的函数一般以avfilter_开头。

## swscale

提供了高级别的图像转换API，例如它允许进行图像缩放和像素格式转换，常见于将图像从1080p转换成720p或者480p等的缩放，或者将图像数据从YUV420P转换成YUYV，或者YUV转RGB等图像格式转换。
这个模块中的接口一般以sws_开头。
*但是这个库的操作大多都十分耗CPU，应减少使用*

## swresample

提供了高级别的音频重采样API。例如它允许操作音频采样、音频通道布局转换与布局调整。这个模块中的代码一般以swr_开头

## avutil 工具函数模块

基本上每个工程都需要又一个util模块做一些杂活儿。这个模块中负责数学计算，内存操作，日志打印，版本号等。这个模块中的函数一般以av_开头

## 播放器ffplay

基于SDL封装的一个播放器。

*有时通过源代码编译生成ffplay不一定能够成功，因为ffplay在旧版本时依赖于SDL-1.2，而ffplay在新版本时依赖于SDL-2.0，需要安装对应的SDL才能生成ffplay*

## ffprobe 多媒体分析工具

ffprobe是一个非常强大的多媒体分析工具，可以从媒体文件或者媒体流中获得你想要了解的媒体信息，比如音频的参数、视频的参数、媒体容器的参数信息等。

# 接口使用

ffmpeg的主要工作流程相对比较简单，具体如下

1. 解封装（Demuxing）
2. 解码（Decoding）
3. 编码（Encoding）
4. 封装（Muxing）

其中需要经过6个步骤，具体如下

1. 读取输入源
2. 进行音视频的解封装
3. 解码每一帧音视频数据
4. 编码每一帧音视频数据
5. 进行音视频的重新封装
6. 输出到目标

**学习的最好做法就是亲自写一写代码，下面按照ffmpeg的能力，分下面几个模块写一下demo**

## 音视频封装(muxing)

步骤

- 根据给定的 **格式/输出文件名** 创建一个输出格式上下文AVFormatContext fmt_ctx

```c
const char* filename = "/Users/hj/Downloads/output2.mp4";
AVFormatContext *fmt_ctx;
avformat_alloc_output_context2(&fmt_ctx, NULL, NULL, filename);
```

- 根据fmt_ctx中的codec id找到解码器AVCodec codec

```c
AVCodec *codec = avcodec_find_encoder(codec_id);
```

- 用codec创建一个解码器上下文AVCodecContext codec_ctx

```c
AVCodecContext *codec_ctx = avcodec_alloc_context3(*codec);
```

- 设置codex_ctx的音频/视频编码的详细参数

- 通过codec初始化codec_ctx

```c
avcodec_open2(codec_ctx, codec, &opt);
```

- 用codec_ctx申请一帧数据AVFrame frame

```c
AVFrame *frame = alloc_picture(codec_ctx->pix_fmt, codec_ctx->width, codec_ctx->height);
```

- 为fmt_ctx创建流AVStream stream

```c
AVStream *stream = avformat_new_stream(fmt_ctx, NULL);
```

- 为了兼容新版本FFmpeg的AVCodecparameters结构，将codec_ctx中的信息拷贝到stream的AVCodecParameters codecpar中,

```c
avcodec_parameters_from_context(stream->codecpar, codec_ctx);
```

- 写头部信息到输出文件

```c
avformat_write_header(fmt_ctx, &opt);
```

- 获取一帧原始(未压缩)数据放入tmp_frame

  - 如果是视频，根据给的尺寸、格式，创建一个视频重采样上下文SwsContext sws_ctx，经过缩放得到最终的dst_frame

    ```c
    sws_ctx = sws_getContext(codec_ctx->width, codec_ctx->height, AV_PIX_FMT_YUV420P, codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt, SCALE_FLAGS, NULL, NULL, NULL);
    tmp_frame = alloc_audio_frame(codec_ctx->sample_fmt, codec_ctx->channel_layout, codec_ctx->sample_rate, codec_ctx->frame_size);
    ... fill frame data field ... 
    sws_scale(sws_ctx, (const uint8_t *const *)tmp_frame->data,
                    tmp_frame->linesize, 0, codec_ctx->height,
                    dst_frame->data, dst_frame->linesize);
    ```

  - 如果是音频，创建一个音频重采样SwrContext swr_ctx，并设置好编码参数  

    ```c
    swr_ctx = swr_alloc();
    av_opt_set_int(swr_ctx, "in_channel_count", codec_ctx->channels, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate", codec_ctx->sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    av_opt_set_int(swr_ctx, "out_channel_count", codec_ctx->channels, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", codec_ctx->sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", codec_ctx->sample_fmt, 0);
    swr_init(swr_ctx);
    ```

    ```text
    FFmpeg编码器默认支持输入是输入格式只能是AV_SAMPLE_FMT_FLTP，如果编码的文件是其他格式，比如AV_SAMPLE_FMT_S16 ，就需要进行转换后才能交给编码器编码，使用swr_convert来完成。
    ```

- 将数据frame **编码** 之后放入包AVPacket pkt中，然后将pkt存入输出文件

```c
int got_packet = 0;

avcodec_encode_video2(codec_ctx, &pkt, frame, &got_packet);
or
avcodec_encode_audio2(codec_ctx, &pkt, frame, &got_packet);
```

- 写尾部信息到输出文件

```c
av_write_trailer(fmt_ctx);
```

- 释放之前申请的所有资源：codec_ctx, frame, fmt_ctx, sws_ctx, swr_ctx

```c
avcodec_free_context(&codec_ctx);
av_frame_free(&frame);
sws_freeContext(sws_ctx);
swr_free(&swr_ctx);
```

## 音视频解封装(demuxing) 或者叫 **分离**

一般步骤

- 打开要demuxing的文件，并找出流信息，这些得到的信息会保存在AVFormatContext fmt_ctx中

```c
avformat_open_input(&fmt_ctx, src_filename, NULL, NULL)
avformat_find_stream_info(fmt_ctx, NULL)
```

- 根据类型（视频AVMEDIA_TYPE_VIDEO/音频AVMEDIA_TYPE_AUDIO）找出对应流的索引stream_index，得到流数据AVStream stream

```c
int index = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0)
AVStream *stream = fmt_ctx->streams[index];
```

- 找到stream对应的解码器AVCodec dec，并根据dec创建一个AVCodecContext codec_ctx

```c
AVCodec *dec = avcodec_find_decoder(stream->codecpar->codec_id);
AVCodecContext *codec_ctx = avcodec_alloc_context3(dec);
```

- 将流stream中的参数拷贝到codec_ctx中，并用codec初始化codec_ctx

```c
avcodec_parameters_to_context(*codec_ctx, stream->codecpar)
avcodec_open2(*codec_ctx, dec, &opt)
```

- 处理视频数据的话，申请一块给定尺寸，格式的内存块uint8_t * video_dst_data[4]

```c
av_image_alloc(video_dst_data, video_dst_linesize, width, height, pix_fmt, 1)
```

- 申请一帧AVFrame frame

```c
frame = av_frame_alloc();
```

- 开启循环，从fmt_ctx中读取一个包AVPacket pkt。直到不能取出pkt

```c
av_read_frame(fmt_ctx, &pkt)
```

- 从pkt中解码到一帧数据frame，并保存到文件中
  - 如果是视频数据，解码数据放到frame之后，将frame中的数据拷贝到video_dst_data

  ```c
  avcodec_decode_video2(video_dec_ctx, frame, got_frame, &pkt)
  av_image_copy(video_dst_data, video_dst_linesize, (const uint8_t **)(frame->data), frame->linesize, pix_fmt, width, height);
  fwrite(video_dst_data[0], 1, video_dst_bufsize, video_dst_file);
  ```

  - 如果是音频数据，数据保存在了frame->extended_data[0]中

  ```c
  avcodec_decode_audio4(audio_dec_ctx, frame, got_frame, &pkt);
  fwrite(frame->extended_data[0], 1, unpadded_linesize, audio_dst_file);
  ```

- 重置frame

```c
av_frame_unref(frame);
```

- 释放资源

```c
avcodec_free_context(&video_dec_ctx);
avcodec_free_context(&audio_dec_ctx);
avformat_close_input(&fmt_ctx);
av_frame_free(&frame);
av_freep(video_dst_data);
```

## 音视频转封装(remuxing)

- 通过输入文件名in_filename初始化一个输入格式上下文AVFormatContext ifmt_ctx

```c
avformat_open_input(&ifmt_ctx, in_filename, 0, 0)
```

- 获取格式上下文中的流信息

```c
avformat_find_stream_info(ifmt_ctx, 0)
```

- 通过输出文件名out_filename得到一个输出格式上下文AVFormatContext ofmt_ctx

```c
avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
```

- 遍历ifmt_ctx中的流，在ofmt_ctx中创建对应的流，并设置(拷贝)编码参数AVCodecParameter

```c
avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
```

- 写输出文件头部

```c
avformat_write_header(ofmt_ctx, NULL);
```

- 开始循环，从输入文件(ifmt_ctx)中得到一个数据包AVPacket pkt
  - 随着输入的封装格式与输出的封装格式的差别化，时间戳也需要进行对应的计算改变

  ```c
  av_read_frame(ifmt_ctx, &pkt);
  pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
  pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
  pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
  pkt.pos = -1;
  ```

  - 将适配好时间标签的pkt写入输出文件(ofmt_ctx)

  ```c
  av_interleaved_write_frame(ofmt_ctx, &pkt);
  ```

- 写输出文件尾部

```c
av_write_trailer(ofmt_ctx);
```

- 释放资源

```c
avformat_close_input(&ifmt_ctx);
avformat_free_context(ofmt_ctx);
```

## 视频截取

```c
// 主要是使用seek操作，其余操作remuxing。需要理解一下timestamp的含义
int av_seek_frame(AVFormatContext *s, int stream_index, int64_t timestamp,
                  int flags);
```

## avio内存数据操作

- 读取文件，将文件内容与内存块buffer对应起来

```c
// 将给定的输入文件与申请的内存块buffer对应起来，并返回内存块大小
av_file_map(filename, &buffer, &buffer_size, 0, NULL);
```

- 创建一个AVFormatContext fmt_ctx

```c
fmt_ctx = avformat_alloc_context();
```

- 根据内存块大小buffer_size创建一个I/O上下文AVIOContext avio_ctx，这里可以设置读取内存数据的回调函数read_packet

```c
int avio_ctx_buffer_size = 4096;
uint8_t *avio_ctx_buffer = av_malloc(avio_ctx_buffer_size);
AVIOContext* avio_ctx = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size, 0, &bd, &read_packet, NULL, NULL);

// 其中read_packet原型为
int read_packet(void *opaque, uint8_t *buf, int buf_size)
```

- 关联格式上下文与I/O上下文

```c
fmt_ctx->pb = avio_ctx;
```

- 打开文件，读取输入流的头部信息

```c
// 这一步操作会触发read_packet的调用
avformat_open_input(&fmt_ctx, NULL, NULL, NULL);
```

- 读取fmt_ctx的流信息

```c
avformat_find_stream_info(fmt_ctx, NULL);
```

- 释放资源

```c
avformat_close_input(&fmt_ctx);
if (avio_ctx) {
  av_freep(&avio_ctx->buffer);
  av_freep(&avio_ctx);
}
av_file_unmap(buffer, buffer_size);
```

## 音视频编解码

上面的代码有涉及，不过目前新的ffmpeg版本中弃用了编解码的接口

```c
attribute_deprecated
int avcodec_encode_video2(AVCodecContext *avctx, AVPacket *avpkt,
                          const AVFrame *frame, int *got_packet_ptr);

attribute_deprecated
int avcodec_decode_video2(AVCodecContext *avctx, AVFrame *picture,
                         int *got_picture_ptr,
                         const AVPacket *avpkt);

attribute_deprecated
int avcodec_encode_audio2(AVCodecContext *avctx, AVPacket *avpkt,
                          const AVFrame *frame, int *got_packet_ptr);

attribute_deprecated
int avcodec_decode_audio4(AVCodecContext *avctx, AVFrame *frame,
                          int *got_frame_ptr, const AVPacket *avpkt);
```

取而代之的是下面代码中用到的接口

```c
avcodec_send_frame(enc_ctx, frame);
if (ret < 0) {
    fprintf(stderr, "Error sending a frame for encoding\n");
    exit(1);
}

while (ret >= 0) {
    ret = avcodec_receive_packet(enc_ctx, pkt);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        return;
    else if (ret < 0) {
        fprintf(stderr, "Error during encoding\n");
        exit(1);
    }

    printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
    fwrite(pkt->data, 1, pkt->size, outfile);
    av_packet_unref(pkt);
}
```

## 视频滤镜

就是讲一个输入的文件添加上一个video filter，然后输出

- 打开文件，得到流信息，找到视频流，为视频流创建并初始化一个解码器上下文

```c
// 主要操作
AVFormatContext *fmt_ctx;
AVCodecContext *dec_ctx;
avformat_open_input(&fmt_ctx, filename, NULL, NULL);
avformat_find_stream_info(fmt_ctx, NULL);
av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
dec_ctx = avcodec_alloc_context3(dec);
avcodec_parameters_to_context(dec_ctx, fmt_ctx->streams[video_stream_index]->codecpar);
avcodec_open2(dec_ctx, dec, NULL)
```

- 创建过滤器图AVFilterGraph filter_graph，过滤器AVFilter buffersrc, buffersink，以及过滤器链AVFilterInOut inputs, outputs，并将他们连接起来

```c
// 主要步骤
const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
const AVFilter *buffersink = avfilter_get_by_name("buffersink");
AVFilterInOut *outputs = avfilter_inout_alloc();
AVFilterInOut *inputs  = avfilter_inout_alloc();
AVFilterGraph *filter_graph = avfilter_graph_alloc();
avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, NULL, filter_graph);
avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                    &inputs, &outputs, NULL);
avfilter_graph_config(filter_graph, NULL);
avfilter_inout_free(&inputs);
avfilter_inout_free(&outputs);
```

- 开始循环处理每一帧数据
  - 从输入文件中读入一个数据包AVPacket pkt

  ```c
  while(1) {
    av_read_frame(fmt_ctx, &packet)
  ```

  - 如果是视频流的数据包，那么解包得到一帧数据AVFrame frame

  ```c
    avcodec_send_packet(dec_ctx, &packet);
    while (ret >= 0) {
      ret = avcodec_receive_frame(dec_ctx, frame);
  ```

    - 将frame推入filter_graph, 并取出来得到想要的加了滤镜的数据帧AVFrame filter_frame
    
    ```c
        av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF)

        while (1) {
            ret = av_buffersink_get_frame(buffersink_ctx, filter_frame);
            if (ret < 0)
                break;
            // got filter_frame
            av_frame_unref(filt_frame);
        }
      }
      av_packet_unref(&packet);
    }
    ```

- 释放资源

```c
avfilter_graph_free(&filter_graph);
avcodec_free_context(&dec_ctx);
avformat_close_input(&fmt_ctx);
av_frame_free(&frame);
av_frame_free(&filter_frame);
```

# 硬件加速

## VAAPI (Video Acceleration API)

## QSV (Intel Quick Sync Video)

# 网络流

# 视频缩放

# 音频重采样

# 遇到的一些情况

# ffmpeg 的部分函数被标记为弃用

目前使用ffmpeg之前不再需要注册API了，在新的ffmpeg中，一些之前必须调用的函数被标记为了deprecated：

```c
#if FF_API_NEXT
/**
 * Initialize libavformat and register all the muxers, demuxers and
 * protocols. If you do not call this function, then you can select
 * exactly which formats you want to support.
 *
 * @see av_register_input_format()
 * @see av_register_output_format()
 */
attribute_deprecated
void av_register_all(void);

attribute_deprecated
void av_register_input_format(AVInputFormat *format);
attribute_deprecated
void av_register_output_format(AVOutputFormat *format);
#endif
```

attribute_deprecated的原型是：

```c
#if AV_GCC_VERSION_AT_LEAST(3,1)
#    define attribute_deprecated __attribute__((deprecated))
#elif defined(_MSC_VER)
#    define attribute_deprecated __declspec(deprecated)
#else
#    define attribute_deprecated
#endif
```

这个是C++14标准中新添加的特性，用于标记某一个C++函数为弃用，比如MSVC中：

```c
__declspec(deprecated) void func1(int) {}
```

## 封装时时间戳的校正

在写audio混合video的封装代码时，发现有这么一个代码：

```c
while (encode_audio || encode_video) {
    if (encode_video &&
          (!encode_audio ||
            av_compare_ts(video_stream.next_pts, video_stream.codec_ctx->time_base,
                          audio_stream.next_pts, audio_stream.codec_ctx->time_base) <= 0)
    ) {
        encode_video = !write_video_frame(fmt_ctx, &video_stream);
    } else {
        encode_audio = !write_audio_frame(fmt_ctx, &audio_stream);
    }
}
```

其中av_compare_ts的作用是比较音频流当前数据(video_stream.next_pts代表的frame)和视频流当前数据(audio_stream.next_pts)所代表的时间戳，通过对比如果视频在前则写视频数据，否则写音频数据。参考函数原型：

```c
/**
 * Compare two timestamps each in its own time base.
 *
 * @return One of the following values:
 *         - -1 if `ts_a` is before `ts_b`
 *         - 1 if `ts_a` is after `ts_b`
 *         - 0 if they represent the same position
 *
 * @warning
 * The result of the function is undefined if one of the timestamps is outside
 * the `int64_t` range when represented in the other's timebase.
 */
int av_compare_ts(int64_t ts_a, AVRational tb_a, int64_t ts_b, AVRational tb_b);
```

上面的if判断就是说如果判断返回-1、或者0，那么就开始写视频数据到文件；否则就写音频数据。

需要指出的是，音频与视频的time base一般相差较大，通过该函数返回0的情况微乎其微。比如说视频数据的time base一般是{1, 24}，也就是一秒24个时间基，也可以说1/24秒一帧，或者说一秒有24个视频数据包AVPacket；而音频一般比如说是{1, 44100}，而一个音频的解码包占用的也不是1/44100秒，比如一个典型的PCM声卡包代表1024/44100（大约0.0232）秒的时间段，所以一个音频流需要44100/1024(约等于43.03个)音频数据包AVPacket。

## More

使用ffmpeg写示例代码的时候会发现很多参数不明白什么意思，查找一些书籍之后发现背后是音视频编解码的概念，这个就比较专业了。这些内容列在了附录中。

## 附录

## 参考文档

- [视频压缩：I帧、P帧、B帧](https://blog.csdn.net/huangblog/article/details/8739876) 在写视频封装的时候AVCodecContext的gop_size参数涉及到I帧的概念
- [音频编码简介](https://cloud.google.com/speech-to-text/docs/encoding?hl=zh-cn) 谷歌的文档
- [理解ffmpeg中的pts，dts，time_base](https://blog.csdn.net/bixinwei22/article/details/78770090)

```c
PTS：Presentation Time Stamp。PTS主要用于度量解码后的视频帧什么时候被显示出来
DTS：Decode Time Stamp。DTS主要是标识读入内存中的bit流在什么时候开始送入解码器中进行解码
ffmpeg中引入time_base来度量时间的单位，表示每个单位是多少秒，比如time_base = {1, 9000}表示一个单位表示1秒的1/9000
```

- [FFMPEG 关于对时间戳转换的理解](https://www.xuejiayuan.net/blog/b796d57c3a604b6ab7a41643e8d1b634)