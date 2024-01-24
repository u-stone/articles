[toc]

# 前言

查了一下自己买的MacBook Pro (Retina, 13-inch, Early 2015)（显卡是Intel Iris Graphics 6100）支持的OpenGL版本是4.1，OpenCL是1.2，参考[Mac computers that use OpenCL and OpenGL graphics](https://support.apple.com/en-us/HT202823)。

我的台式机使用的是Intel i6700，支持OpenGL 4.6。Direct X支持12.1，11.4，Shader model为5。Vulkan支持1.1.120。

而《OpenGL编程指南》对应版本的情况如下：
- 第九版（2017年中文版）对应的是OpenGL 4.5
- 第八版（2014年中文版）对应的是OpenGL 4.3
- 第七版（2010年中文版）对应的是OpenGL 3.0 和 3.1

考虑到Mac自10.14开始已经抛弃OpenGL，而且mac上又有一堆问题。

本想看第九版，但是资源不好找，就看第八版吧。第九版相比第八版变动也不大。

目前维护OpenGL的Khronos组织已经开始主推[Valkan](https://en.wikipedia.org/wiki/Vulkan_(API))了。但网上有人评论是说OpenGL依然是图形入门，懂一些更好。

单纯的OpenGL书只能系统地介绍一些知识，具体应用方面，还涉及到矩阵计算，空间几何变换，GLSL的语法和内置函数的使用，等等。这个需要通过写demo，修改代码来掌握。

# GLSL校验工具

GLSL语言校验器：khronos提供的 [OpenGL / OpenGL ES Reference Compiler](https://www.khronos.org/opengles/sdk/tools/Reference-Compiler/)，可用来检查你写的shader有哪些错误，或者兼容性问题。

# 调试工具

参考文章：[调试](https://learnopengl-cn.readthedocs.io/zh/latest/06%20In%20Practice/01%20Debugging/)

OpenGL调试GLSL的时候有很大的麻烦，如果要写代码打印的话，需要将GPU中的数据输出到CPU然后再打印出来。还有就是需要借助工具：

参考khronos的wiki：[Debugging Tools](https://www.khronos.org/opengl/wiki/Debugging_Tools)

基本上就是下面这几种：

1. Debug Output
2. RenderDoc
3. BuGLe
4. AMD CodeXL
5. APITrace
6. GLIntercept
7. GLSL-Debugger
8. Xcode tools
9. Vogl
10. AMD GPU PerfStudio
11. NVIDIA Nsight Graphics
12. NVIDIA Nsight VSE

两类：厂商提供的SDK Tools，还有开源的工具。

# 网站
- [glfw](https://www.glfw.org/)
- [glew](http://glew.sourceforge.net/)
- [B站上的一个教学视频](https://www.bilibili.com/video/av68903616?p=1)
- [教程：learnopengl](https://learnopengl.com/)
