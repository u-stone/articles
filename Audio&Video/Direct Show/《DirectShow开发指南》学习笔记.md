
```
1. Filter大致分为：Source Filter、Transform Filter、和Render Filter
2. Filter由Filter Graph管理，Filter是一种COM组件，会注册到系统中。系统中已经注册的Filter可以使用GraphEdit查看。
3. 在DirectShow中有描述媒体信息的结构：AM_MEDIA_TYPE
4. 媒体的播放需要把若干Filter拼接在一起，Filter的连接实际上有Filter上的Pin完成，这就是IPin接口。
5. Filter必须在一个Filter Graph中才可以拼接在一起。
Filter最好注册之后再使用（也有不注册使用的方法），注册的方法是：
LoadLibrary这个filter
得到DllRegisterServer或者DllUnregisterServer方法并调用
OK！
想要知道某个Filter有没有注册，直接调用COCreateInstance创建这个对象，根据返回值成功与否判断


6. 使用上一个Filter的Out Pin调用IPin::Connect（下一个Filter的IPin指针），如果成功，那么还要下一个Filter的In-Pin接受，双方都可以连接，方才成功。
Pin的连接是连接双方使用媒体类型的一个“协商”过程。
详细步骤是：
首先由out-pin对in-pin发起询问，指定的媒体格式是否由in-pin支持？
      如果这种格式是完整格式，那么直接向in-pin尝试连接
      如果不是完整格式，需要给in-pin这种格式的数据，让其确定是否支持
                如果支持，那么尝试连接
      如果尝试连接失败，那么直接返回；如果尝试成功，那么继续
      Pin之间协商数据由谁来管理、
7.  并不是所有Filter的Pin连接都需要手动完成的，也可以使用智能连接：IGraphBuiler::AddSourceFilter，IGraphBuilder::Connect，IGraphBuilder::Render， IGraphBuiler::RenderFile；并且，通过修改Filter的Merit值（可以理解为连接使用的优先级）来修改智能连接的具体实现。
IGraphBuiler::AddSourceFilter
    过程：根据原文件路径分析。
        如果路径中有协议名，那么查找注册表中HEKY_CLASSES_ROOT/<协议名>下查找Extensions下匹配的扩展名；不成功的话就使用Source Filter下指定的CLSID；
        如果没有协议名，就查找HKEY_CLASSES_ROOT\MediaType\Extensions下查找匹配的扩展名。找到后，就使用对应的CLSID；找不到就使用HKEY_CLASSES_ROOT\MediaType下{E436EB83-524F-11CE-9F53-0020AF0BA770}的子项做字节校验；。如果还是不能匹配到，那么就创建一个File Source的Filter。
IGraphBuiler::RenderFile
    过程：首先找到正确的Filter，过程类似IGraphBuiler::AddSourceFilter；从Source Filter的各个out-pin开始，进行智能连接，把剩余的Pin串起来；智能连接的过程：
如果out-pin支持IStreamBuilder，那么交给IStreamBuilder::Render
使用Filter Graph Manager的内部缓存中饭的Filter进行连接尝试
使用当前Filter Graph中还没有完全连接的Filter进行连接尝试
使用IFilterMapper2::EnumMatchingFilters搜索注册表，尝试连接所有Merit值大于MERIT_DO_NOT_USE的Filter。Merit值越高，对应Filter被使用的概率越大。
IGraphBuilder::Render
    从当前Filter Graph的某个Filter的out-Pin开始进行智能连接，算法与上述IGraphBuiler::RenderFile类似。
IGraphBuilder::Connect
    这个方法在一对out-pin和in-pin之间做连接，先尝试直连，不成功的话，尝试中介Filter，这个Filter的选择是一个智能连接过程，类似IGraphBuilder::RenderFile。

还有一部分动态重建：


8. Pin之间的数据传输，一般使用sample的形式，具体地一般是IMemInputPin接口。
上一级的out-pin使用一个与下一级的in-pin共用的Allocator，通过这个Allocator创建、管理若干sample，并使用“通知”的方式传递给下一级pin。真正的数据拷贝发生在Filter内部，比如说某个Filter对数据进行了处理，得到新的sample，并传送下去。
9. 数据的传送模式有：推模式push和拉模式pull；前者多发生在采集设备上，后者多发生在文件数据源上。
对于推模式：一般由source Filter的out-pin实现IAsyncReader接口向下一级Filter推送数据；对于拉模式，一般由下一级的Filter实现IMemInputPin来I拉取数据，拉取的方式一般是使用一个专门的线程来不断地取数据。不论哪种模式，source Filter之后的Filter一般都只使用拉模式。
除此之外，还有一种IOverlay接口。实现这种接口的pin和下一级Filter的in-pin之间不会有数据传输，该接口可以直接写入显卡内存，并且设置渲染窗口的位置。
10. Filter的状态、及转换关系有：停止<->暂停<->运行。从停止到暂停，是数据准备阶段，从暂停到运行时数据渲染阶段。对于拉模式来说，从停止到暂停状态就要启动数据传送；而对于推模式，并不需要，只有在运行的时候才需要启动数据传送。
11. seek的实现：IMediaSeeking，实际由Filter实现。一般在推模式下source Filter的out-pin，拉模式的parser Filter或者splitter Filter的out-pin上实现真正的seek。
12. 质量控制：    调整数据传送快慢的。
13. 音视频同步：具体的方式是Filter graph 提供一个公共的参考始终，每一个sample都被打上时间戳；如果到达的时间比较晚，那么马上播放，并且发送加快sample的消息；早了，就等待，并且发送消息要求sample慢些。Directshow中参考时钟的计时精度是100ns。
一般来说，推模式的source Filter或者拉模式的parser Filter给sample打时间戳。
14. DMO：微软推荐使用DMO替换DirectShow Transform Filter。
DMO对象实现了IMediaObject接口。
DMO使用流的概念，弃用Pin。
DMO不需要：连接Pin，分配sample内存，推处理完的数据，与Filter Graph进行事件交互，处理多线程同步问题。这些由DMO的使用者处理。


15. 使用DirectShow开发的一般过程：
创建Filter Graph Manager（IGraphBuilder）
根据具体情况创建一条Filter链路；有些简单的方法可以使用智能连接
对各个Filter进行控制，完成Filter Graph Manager与程序的事件、UI交互
16. DES非线性编辑，用于编辑视频。
17. DVD播放：DVD的文件系统使用MicroUDF标准，光盘中一般有两个文件夹：VIDEO_TS和AUDIO_TS，一般VIDEO_TS中才有数据。VIDEO_TS中有文件：.vob, .ifo, .bup。其中，.vob用来保存所有mpeg2格式的音视频数据，ifo中是控制vob文件的，包含了何时以及如何播放vob文件的控制信息；bup是ifo的一个备份。有些影片长度超过1G，就需要放在多个vob文件中了。vob和ifo的文件名的格式是：VTS_xx_y.vob，VTS_xx_y.IFO。其中xx是节目编号（01-99），y是0-9的编号。DVD的内容还可以根据设置的PML（Parental Management Level），级别1-8，防止孩子在未经父母同意的情况下观看成人电影。
DirectShow没有提供播放DVD的Filter

18. SDK中提供的例子
Source Filter：拉模式 asynbase，async，memfile；推模式 ball，
Transform Filter：contrast，NullInPlace
Render Filter：SampVid，dump
DMO的例子：DMOSample
枚举系统音视频设备、Filter、midi设备（都是系统中注册的Filter）：SysEnum、Mapper、DMOEnum
播放器实例：PlayWndASF，这个例子有完整的播放流程，具有很高的参考价值。

19. Filter的4个重要属性：
注册名：HQ MPEG-2 Video DEcoder
类型：e.g transform Filter
CLSID：e.g CLSID_FilterMpeg2VD
Metrit值：e.g METRIT_PREFERRED

20. GraphEdit的使用，可以：
拖进去一个文件，看当前系统中的Filter是否支持能顺利播放该文件
开发一个Filter，拖进去一个支持的文件，验证是否正常工作，做各种测试


21. SDK中实例的分析
另起一篇《DirectShow开发指南》 SDK实例代码学习

22. 好了，看完上面的资料，接下来就需要写代码体验一下了，要不无法进一步掌握DirectShow。
```

原文由 2015.10.27 创建，现在倒腾到git上维护。