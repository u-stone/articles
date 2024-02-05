[TOC]

# 本文介绍学习音视频通信、及ffmpeg相关的知识点

## Mac上手动编译ffmpeg

ffmpeg使用了make来维护代码，所以不能使用cmake gui这类可视化的工具来编译。

在Mac上编译的话比较简单，可以使用brew帮忙下载依赖的第三方工具。同unix系统及其派生系统一样，都是使用下面的指令进行编译和安装的：

```
./configure
make
make install
```

并且还可以指定安装位置。具体请参考ffmpeg官网的一篇文章 [Generic Compilation Guide](https://trac.ffmpeg.org/wiki/CompilationGuide/Generic)

在各个平台上碰到问题都可以参考网页: [FFmpeg Compilation Guide](https://trac.ffmpeg.org/wiki/CompilationGuide)
在我的电脑上，使用brew安装了ffmpeg之后，再编译从源码ffmpeg会提示需要安装: yasm和pkg-config，需要使用homebrew来安装。

如果想要研究ffmpeg最好是可以自己编译一下，这样有调试信息的库方便调试。在Mac上编译ffmpeg的时候，可以设置一下编译选项中的prefix，比如我设置的是：
```
./configure --prefix=/Users/hj/ffmpeg/build --enable-shared
```

这样在make install的时候就会将生成的文件拷贝进指定的prefix目录的相应子目录下：

```
➜  build git:(release/4.1) ✗ l
total 0
drwxr-xr-x   6 hj  staff   192B Jul  4 16:47 .
drwxr-xr-x  49 hj  staff   1.5K Jul  4 16:47 ..
drwxr-xr-x   5 hj  staff   160B Jul  4 16:47 bin
drwxr-xr-x   9 hj  staff   288B Jul  4 16:47 include
drwxr-xr-x  10 hj  staff   320B Jul  4 16:47 lib
drwxr-xr-x   4 hj  staff   128B Jul  4 16:47 share
```

在我的电脑上编译一次ffmpeg的时间大概是12分钟。

## 示例代码

- [官方示例代码](https://ffmpeg.org/doxygen/trunk/examples.html)
- 雷霄骅写的例子 [Lei Xiaohua's learning resource about video/audio technics](http://leixiaohua1020.github.io/#ffmpeg-development-examples)
- 还有一个方法是先熟悉ffmpeg的命令行，然后根据命令参数搜索源码来学习

## 视频

- [王纲的视频教程: 跟我学ffmpeg](http://www.chungen90.com/index.php?m=text&a=index&classify_id=208) 这个系列在[B站](https://www.bilibili.com/video/av53895486/)和[爱奇艺](https://www.iqiyi.com/u/1426749687/feeds)上都有
- [基于FFmpeg+SDL的视频播放器的制作——雷霄骅](https://www.bilibili.com/video/av9927626)

## 电子书

目前来说，找到的一些ffmpeg的电子书基本都是重点介绍命令行功能。毕竟ffmpeg的初始功能就是这个。

- [新一代高效视频编码H.265HEVC：原理、标准与实现](https://download.csdn.net/download/zhousiwei/10399111)
- Iain E G Richardon写的《h.264和mpeg-4视频压缩》 [百度网盘链接](https://pan.baidu.com/s/1CgGi7wnEndFKZJQr423wlA) 提取码: b6iq
- 我收集的一个音视频书籍的集合：[百度网盘链接](https://pan.baidu.com/s/14sRXztH-2iysfEFgPHIBsg) 提取码:ntn6，里面有一个ffmpepg的电子书: **《ffmpeg基础库编程开发》20140307**
- [Jpeg2000图像压缩基础、标准和实践](https://issuu.com/xiuxiuebook/docs/jpeg2000___________________________)
- [FFmpeg从入门到精通](https://read.douban.com/ebook/49786757/) 作者：刘歧 赵文杰，该书在豆瓣上评价不高，但是入门的书籍较少，较旧，该书是2018年的较新，能入门就好了。另，豆瓣阅读上有活动¥9.99，京东上有活动的时候是¥15.

## 文档

- [ffmpeg_develop_doc](https://github.com/0voice/ffmpeg_develop_doc)
- [9份ffmpeg典藏文档文档，比官方都详细](https://zhuanlan.zhihu.com/p/456072886)
- 即时通讯网中的一个系列文章 [即时通讯音视频开发](http://www.52im.net/thread-228-1-1.html) 1 - 18。 其实还有好多其他的主题，想要入门通信领域的话，可以关注这个网站的其他主题。其中的第6篇中介绍了音频编解码应该看的书，极具参考价值。第14篇介绍了实时音视频通信涉及到的传输协议，可以系统地了解一二。
- [H.265（HEVC）深度解析](https://blog.csdn.net/xiaojun111111/article/details/61199130)
- [H264标准(中文）](https://download.csdn.net/download/qq_37301839/10750036)
- [研究音频编解码要看什么书](https://www.cnblogs.com/gaozehua/archive/2012/04/10/2440219.html)
- 卢俊，七牛直播 SDK 技术负责人在知乎专栏写的[【音视频开发】开发小白如何成为音视频专家？](https://zhuanlan.zhihu.com/p/31717622) 这个回答中同时还描述了职业瓶颈的一些看法，很有参考价值。
- 如果研究生阶段方向是音视频编解码，本科期间应该打好哪些基础？ - [张晖的回答](https://www.zhihu.com/question/27005982/answer/34945817) - 知乎
- 如果研究生阶段方向是音视频编解码，本科期间应该打好哪些基础？ - [武晓阳的回答](https://www.zhihu.com/question/27005982/answer/51158064) - 知乎
- 如果研究生阶段方向是音视频编解码，本科期间应该打好哪些基础？ - [殷汶杰的回答](https://www.zhihu.com/question/27005982/answer/590029948) - 知乎。这个回答结合来目前的职位要求，比较有实际价值。

## 网站

- ffmpeg官网的 [wiki](https://trac.ffmpeg.org/) **这是一个必须重视的网站**
- ffmpeg文档中提到的一个网站 **[MultimediaWiki](https://wiki.multimedia.cx/index.php)** *它致力于打造一个收集各种与多媒体有关的信息获取和编辑的地方*
- [即时通信网](http://www.52im.net/) 里面有好多大牛
- LiveVideoStack的的一些专题的 [演讲回顾](http://sh2019.livevideostack.com/article) 我们沪江的唐小浙是LiveVideoStack的联席主席之一，*大招*在里面是讲师。
- [xiph](https://www.xiph.org) 一个制定免费开源音视频编码实现的非盈利机构，制定了Theora 视频编码、Vorbis 音频编码、Ogg 容器格式。还提供了一些测试资源 [Xiph.org Test Media](https://media.xiph.org)
- [SDL](http://www.libsdl.org) 一个多媒体库，封装了计算机外设接口，可视化接口

## 博客
- [雷霄骅(leixiaohua1020)的专栏](https://blog.csdn.net/leixiaohua1020)
- 殷汶杰CSDN博客 [Workshop of Wenjie.Yin](https://blog.csdn.net/shaqoneal) 博主有几个系列的文章都不错，同时在CSDN上还有一个视频课程讲解H.264的
- 斯坦福教授[Julius Orion Smith III](https://ccrma.stanford.edu/~jos/)教授主页
- 刘华平的[博客](https://www.cnblogs.com/huaping-audio/)，读了博主的部分文章，发现音频编码居然比视频编码复杂，比较惊讶。
- 腾讯云上的专栏 [流媒体](https://cloud.tencent.com/developer/column/4902)
- 博客园上的 [灰色飘零](https://www.cnblogs.com/renhui/category/1165051.html)

## 工具

视频流分析工具，其中有的是商业的，有的是免费的。

- Elecard Stream Analyzer (commercial)
- CodecVisa (commercial)
- Intel Video Pro Analyzer (commercial)
- AOMAnalyzer (free, AV1/VP9 video)

*上面4个工具摘自 [FFmpeg Encoding and Editing Course 第53页](https://slhck.info/ffmpeg-encoding-course/#/53)*

- [mp4info](https://www.bento4.com/documentation/mp4info/) 一个可视化的查看mp4文件信息的工具


## 相关标准化组织

- [ITU](https://www.itu.int/en/Pages/default.aspx)：International Telecommunications Union
- [VCEG](https://www.itu.int/en/ITU-T/studygroups/2017-2020/16/Pages/video/vceg.aspx): Video Coding Experts Group is a working group of the ITU Telecommunication Standardization Sector (ITU-T) concerned with video coding standards.
- [VQEG](https://www.its.bldrdoc.gov/vqeg/video-datasets-and-organizations.aspx) Video Quality Experts Group (VQEG) 有一个免费视频资源，可用来测试。
- [ISO](): International Standards Organization
- [MPEG](https://mpeg.chiariglione.org): Motion Picture Experts Group
- [3GPP](https://www.3gpp.org): 3rd Generation Partnership Project (3GPP). The Mobile Broadband Standard

## 相关专业以及参考资料

与音视频编解码相关的专业有通信专业，电子信息专业……

与之相关的课程有：信号与系统，数字信号处理，信息论。弱相关的有数电模电、通信原理……

参考殷汶杰在知乎上的回答，相关的职位有：

- 编解码算法**研究**（要求博士及以上学位），职位较少，偏研究类型
- 算法**工程实现** 属于工程领域。就是要熟悉ffmpeg，x264 x265这些开源项目，要求了解主流图像视频编解码&压缩原理
- 音视频**应用开发**，属于工程领域。就是在不同的平台上实现业务逻辑，要求是熟悉平台（PC、移动、及不同的操作系统）相关技术、概念、框架。同一个音视频通信模型的概念在不同平台上有不一样的实现，比如DirectX家族、OpenGL、GDI，speexdsp、libfaac、opus、speex、ffmpeg，还有计算机网络相关知识，TCP/IP相关协议RTMP、RSTP、HLS、HDL等……在采集、传输、处理、渲染4个环节都有很多具体的平台实现。参考[【音视频开发】开发小白如何成为音视频专家？](https://zhuanlan.zhihu.com/p/31717622)  

## 直播测试流
M3U8 流
- CCTV 1 直播流 ：http://ivi.bupt.edu.cn/hls/cctv1hd.m3u8
- CCTV 6 直播流 ：http://ivi.bupt.edu.cn/hls/cctv6hd.m3u8
- CCTV 8 直播流 ：http://ivi.bupt.edu.cn/hls/cctv8hd.m3u8

## 常见格式
- 来自 [灰色飘零](https://www.cnblogs.com/renhui/category/1165051.html) 的博客分类 **[多媒体文件格式](https://www.cnblogs.com/renhui/category/1393420.html)** 包含了mp4, flv, m3u8, ts文件格式，以及分析工具

