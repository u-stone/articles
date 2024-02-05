[TOC]

*ffmpeg中的方法太多了，而且因为是c的代码，结构上没有C++以类的方式组织代码方便，所以需要归类一下API，增加一下熟悉程度*

# AVFormatContext 音视频格式上下文

视频的尺寸，格式，码率，time base，编解码器ID，音频的采样率等信息都会保存在这里，当然，对于部分信息来说，这里并不是唯一保存的地方。ffmpeg中大量使用了这种将音频和视频信息混合在一起的struct，而且还要区分编码的时候设置，还是解码的时候设置，由用户设置，还是由ffmpeg初始化，所以使用API的时候需要注意阅读函数注释。

```c
// 创建一个格式上下文
AVFormatContext *avformat_alloc_context(void);

// 根据给定的信息(输出格式/格式名/输出文件名)，创建一个输出格式上下文
int avformat_alloc_output_context2(AVFormatContext **ctx, AVOutputFormat *oformat,
                                   const char *format_name, const char *filename);

// 打开一个流并读取头部信息，必须要使用avformat_close_input()关闭。
int avformat_open_input(AVFormatContext **ps, const char *url, AVInputFormat *fmt,
                                    AVDictionary **options);
void avformat_close_input(AVFormatContext **s);

// 用来释放各种格式上下文，以及其包含的流stream
void avformat_free_context(AVFormatContext *s);

// 读取一个packet或者文件(这些信息来自参数AVFormatContext)来获取流信息，对于一些没有头部信息的文件格式(e.g. MPEG)来说，这个函数非常有用。
int avformat_find_stream_info(AVFormatContext *ic, AVDictionary **options);

// 打印出格式的详细信息
void av_dump_format(AVFormatContext *ic, int index, const char *url, int is_output);

// 找出文件中符合给定类型type的最合适的流
int av_find_best_stream(AVFormatContext *ic,
                        enum AVMediaType type,
                        int wanted_stream_nb,
                        int related_stream,
                        AVCodec **decoder_ret,
                        int flags);
```

# AVOutputFormat

# AVCodec

## 查找解码器：根据给定的ID或者名字

程序里面的编解码器都是find得到的

```c
AVCodec *avcodec_find_encoder(enum AVCodecID id);
AVCodec *avcodec_find_encoder_by_name(const char *name);
AVCodec *avcodec_find_decoder(enum AVCodecID id);
AVCodec *avcodec_find_decoder_by_name(const char *name);
```

# AVCodecContext 解码器上下文

```c
// 创建
AVCodecContext *avcodec_alloc_context3(const AVCodec *codec);

// 释放
void avcodec_free_context(AVCodecContext **avctx);

// 使用给定的AVCodec初始化AVCodecContext，一般调用了avcodec_alloc_context3之后，会调用这个函数
int avcodec_open2(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);

// 使用AVCodecParameters初始化，比如打开了一个文件，用这个方法拷贝文件流的中解码的信息到解码器codec，
// 一般在解码的时候用
int avcodec_parameters_to_context(AVCodecContext *codec,
                                  const AVCodecParameters *par);

// 比如要将数据保存到文件中，使用这个函数将程序设置的编码参数拷贝到par中，
// 一般在编码的时候用
int avcodec_parameters_from_context(AVCodecParameters *par,
                                    const AVCodecContext *codec);
```

# AVCodecParameters 音视频的编解码参数

```c
// 创建
AVCodecParameters *avcodec_parameters_alloc(void);

// 释放
void avcodec_parameters_free(AVCodecParameters **par);

// 拷贝
int avcodec_parameters_copy(AVCodecParameters *dst, const AVCodecParameters *src);
```

# AVStream 流

```c
// 为媒体文件创建一个流
AVStream *avformat_new_stream(AVFormatContext *s, const AVCodec *c);
```

# AVFrame 一帧未经压缩的音频/视频数据

```c
// 新建一个默认的frame，这个仅仅是申请一个AVFrame大小的内存，不会申请数据空间，
// 需要用ab_frame_get_buffer来申请。
AVFrame *av_frame_alloc(void);

// 必须释放
void av_frame_free(AVFrame **frame);

// 为音/视频申请一块儿数据内存，但是，需要首先设置一些必要的参数
int av_frame_get_buffer(AVFrame *frame, int align);

// 从流中读取一个数据包
int av_read_frame(AVFormatContext *s, AVPacket *pkt);
```

# AVPacket 经过压缩的音/视频数据包

```c
// 申请，克隆，释放
AVPacket *av_packet_alloc(void);
AVPacket *av_packet_clone(const AVPacket *src);
void av_packet_free(AVPacket **pkt);

// 初始化
void av_init_packet(AVPacket *pkt);

// 将一个数据包输出到文件
int av_write_frame(AVFormatContext *s, AVPacket *pkt);

// 有多个流的时候，将数据包输出到文件，这个函数会缓存pkt数据以保证正确的顺序
int av_interleaved_write_frame(AVFormatContext *s, AVPacket *pkt);
```

# 图片数据

```c
// 根据给定的尺寸+格式，申请一块内存存放图片数据
int av_image_alloc(uint8_t *pointers[4], int linesizes[4],
                   int w, int h, enum AVPixelFormat pix_fmt, int align);
```

# SwrContext 音频采样上下文

```c
// 创建
struct SwrContext *swr_alloc(void);

// 释放
void swr_free(struct SwrContext **s);

// 初始化
int swr_init(struct SwrContext *s);

// 关闭 与swr_init相对应
void swr_close(struct SwrContext *s);

// 转换
int swr_convert(struct SwrContext *s, uint8_t **out, int out_count,
                                const uint8_t **in , int in_count);
```

因为SwrContext与其他上下文不一样，它是不透明的，需要使用一组操作函数来设置和获取值：

```c
// 设置
int av_opt_set         (void *obj, const char *name, const char *val, int search_flags);
int av_opt_set_int     (void *obj, const char *name, int64_t     val, int search_flags);
int av_opt_set_double  (void *obj, const char *name, double      val, int search_flags);
int av_opt_set_q       (void *obj, const char *name, AVRational  val, int search_flags);
int av_opt_set_bin     (void *obj, const char *name, const uint8_t *val, int size, int search_flags);
int av_opt_set_image_size(void *obj, const char *name, int w, int h, int search_flags);
int av_opt_set_pixel_fmt (void *obj, const char *name, enum AVPixelFormat fmt, int search_flags);
int av_opt_set_sample_fmt(void *obj, const char *name, enum AVSampleFormat fmt, int search_flags);
int av_opt_set_video_rate(void *obj, const char *name, AVRational val, int search_flags);
int av_opt_set_channel_layout(void *obj, const char *name, int64_t ch_layout, int search_flags);


// 获取
int av_opt_get         (void *obj, const char *name, int search_flags, uint8_t   **out_val);
int av_opt_get_int     (void *obj, const char *name, int search_flags, int64_t    *out_val);
int av_opt_get_double  (void *obj, const char *name, int search_flags, double     *out_val);
int av_opt_get_q       (void *obj, const char *name, int search_flags, AVRational *out_val);
int av_opt_get_image_size(void *obj, const char *name, int search_flags, int *w_out, int *h_out);
int av_opt_get_pixel_fmt (void *obj, const char *name, int search_flags, enum AVPixelFormat *out_fmt);
int av_opt_get_sample_fmt(void *obj, const char *name, int search_flags, enum AVSampleFormat *out_fmt);
int av_opt_get_video_rate(void *obj, const char *name, int search_flags, AVRational *out_val);
int av_opt_get_channel_layout(void *obj, const char *name, int search_flags, int64_t *ch_layout);
```

# SwsContext 视频采样上下文

```c
// 创建
struct SwsContext *sws_getContext(int srcW, int srcH, enum AVPixelFormat srcFormat,
                                  int dstW, int dstH, enum AVPixelFormat dstFormat,
                                  int flags, SwsFilter *srcFilter,
                                  SwsFilter *dstFilter, const double *param);

// 缩放图像
int sws_scale(struct SwsContext *c, const uint8_t *const srcSlice[],
              const int srcStride[], int srcSliceY, int srcSliceH,
              uint8_t *const dst[], const int dstStride[]);
```

# AVDictionary

自定义的字典类型，一般用来做传递配置参数用

```c
int av_dict_copy(AVDictionary **dst, const AVDictionary *src, int flags);
void av_dict_free(AVDictionary **m);
```

# AVIOContext

```c
// 创建并初始化一个带缓存的I/O上下文
AVIOContext *avio_alloc_context(
                  unsigned char *buffer,
                  int buffer_size,
                  int write_flag,
                  void *opaque,
                  int (*read_packet)(void *opaque, uint8_t *buf, int buf_size),
                  int (*write_packet)(void *opaque, uint8_t *buf, int buf_size),
                  int64_t (*seek)(void *opaque, int64_t offset, int whence));
```

# AVFilterContext 过滤器上下文

# AVFilter 过滤器

# AVFilterInOut 音视频过滤器节点

# AVFilterGraph 过滤器图

# 错误信息

- ffmpeg的方法一般是返回一个负值表示错误，使用下面的函数可以得到该错误值对应的文本信息。

```c
// 其实是一个宏定义
av_err2str(errnum)
```

# 文件

```c
// 申请流的私有数据，并将流的头部写入到输出文件
int avformat_write_header(AVFormatContext *s, AVDictionary **options);

// 写输出文件的尾部
int av_write_trailer(AVFormatContext *s);
```
