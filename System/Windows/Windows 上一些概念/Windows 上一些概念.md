[toc]

# DPI

[DPI and device-independent pixels](https://learn.microsoft.com/en-us/windows/win32/learnwin32/dpi-and-device-independent-pixels) 上有一个很好的解释，同时回顾了一下历史：

在传统的出版业中，印刷的大小按照点(point)来计量，一个点大约是 1/72 英寸，也就是72个点大约就是一英寸；我们常说的12号字体，就是 1/6(12/72)英寸。

但是到计算机显示器中，由于像素的大小跟显示器的类型有关，无法通过英寸来简单地计量，所以有了逻辑尺寸用来衡量字体的大小。一直以来Windows上通过96个像素点作为1英寸来做转换，所以，72号字体对应显示为1英寸。

传统的DPI含义是每英寸多少个墨水点，但是到了计算机显示中，这个一般描述为每逻辑英寸多少像素。只不过DPI的术语迁移过来，沿用至今。

但是，现在的显示器越来越高清，到2017年，300 DPI的显示器比比皆是，如果还是用老的对应关系显示，字体显示就会很小，所以Windows上现在DPI出现了 144 (150% DPI)等缩放比例的概念。比如在144 DPI下，也就是UI缩放150%，也就是使用更多的物理像素去对应1英寸的逻辑尺寸。这样处理之后，显示的内容更加细腻。

**GDI** 绘图以像素为单位。这意味着如果您的程序被标记为 DPI 感知，并且您要求 GDI 绘制一个 200 × 100 的矩形，则生成的矩形在屏幕上将是 200 像素宽和 100 像素高。但是，GDI 字体大小会缩放到当前的 DPI 设置。换句话说，如果您创建一个 72 号字体，字体大小在 96 DPI 时为 96 像素，而在 144 DPI 时为 144 像素。这是使用 GDI 以 144 DPI 呈现的 72 点字体。

**Direct2D** 自动执行缩放以匹配 DPI 设置。在 Direct2D 中，坐标以称为设备无关像素(DIP: device-independent pixels)的单位进行测量。DIP 定义为逻辑英寸的 1/96 。在 Direct2D 中，所有绘图操作都在 DIP 中指定，然后缩放到当前 DPI 设置。

|DPI setting |	DIP size|
|--|--|
|96	| 1 pixel
|120	| 1.25 pixels
|144	| 1.5 pixels

例如，如果用户的 DPI 设置为 144 DPI，并且您要求 Direct2D 绘制一个 200 × 100 的矩形，则该矩形将为 300 × 150 物理像素。此外，DirectWrite 以 DIP 而非磅为单位测量字体大小。要创建 12 磅字体，请指定 16 个 DIP（12 磅 = 1/6 逻辑英寸 = 96/6 DIP）。在屏幕上绘制文本时，Direct2D 将 DIP 转换为物理像素。该系统的好处是无论当前的 DPI 设置如何，文本和绘图的测量单位都是一致的。

提醒一句：鼠标和窗口坐标仍然以物理像素而不是 DIP 给出。例如，如果您处理WM_LBUTTONDOWN消息，则鼠标按下位置以物理像素为单位给出。要在该位置绘制一个点，您必须将像素坐标转换为 DIP。


# Windows 上的内存分配

微软文档 [Comparing Memory Allocation Methods](https://docs.microsoft.com/en-us/windows/win32/memory/comparing-memory-allocation-methods) 中提到Windows上所有内存分配方式。

*   自从32位Windows开始，GlobalAlloc、LocalAlloc底层使用的都是HeapAlloc。所以要尽可能地使用HeapAlloc；同时，对应的内存释放应该由对应的方法来做，不能混用。
*   VirtualAlloc 可以设置一些特别的选项，内存分配使用的是页分配粒度，同时也会带来更大的内存使用率。
*   malloc 函数依赖于运行时库
*   new 依赖于编译器和语言
*   CoTaskMemAlloc 是可以在C、C++、VB中都能使用的，基于COM的app中共享内存的唯一方法。对应地有 CoTaskMemFree 释放内存。
*   参考问题：[What's the differences between VirtualAlloc and HeapAlloc?](https://stackoverflow.com/questions/872072/whats-the-differences-between-virtualalloc-and-heapalloc)

# Windows性能工具包

## 官网地址

https://docs.microsoft.com/en-us/windows-hardware/test/wpt/

> WPR/WPA工具，目前开发的还不够健壮，老是会crash。

安装好Windows Kit之后，WPR工具存放在 `C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit` 下。可以直接打开 `WPRUI.exe` 开始采集、分析。或者直接在开始菜单中搜索 "wprui"，也可以打开 Windows Performance Recorder。

参考 [Windows 性能记录器常见方案](https://docs.microsoft.com/en-us/windows-hardware/test/wpt/windows-performance-recorder-common-scenarios) 和 [Windows 性能分析器常见方案](https://docs.microsoft.com/en-us/windows-hardware/test/wpt/windows-performance-analyzer-common-scenarios) 学习如何使用一些方案来分析问题。如果运行的exe有pdb符号的话，还可以加载符号查看调用堆栈。

观察Visual Studio的Performance Profile关于内存信息的采集和收集信息过程，可以发现，其底层使用的应该也是WPR，最后都是整理etl数据。但是Visual Studio中一旦调用到了非项目使用的代码，都直接以“外部代码”来标识call stack，导致无法看到内存泄露的时，实际的调用堆栈是在哪里。 所以一旦碰到想要看所谓的“外部代码”的情况时，就需要使用wpr来观察堆栈。尤其是对于那些系统层面的调用。wpa在分析数据的时候可以设置系统的symbol，以便观察对系统模块的调用过程。

## 其他资料
- [Heap Snapshots–Tracing All Heap Allocations](https://randomascii.wordpress.com/2019/10/27/heap-snapshots-tracing-all-heap-allocations/)
- 微软为 `.Net` 程序开发了一个查找CPU和内存性能问题的工具 [microsoft/perfview](https://github.com/Microsoft/perfview/)，有一个教程网站：[PerfView Tutorial](https://docs.microsoft.com/zh-cn/shows/perfview-tutorial/)