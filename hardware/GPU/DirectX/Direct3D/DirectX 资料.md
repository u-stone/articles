[TOC]

# 版本选择

*   D3D 9 => XP and later
*   D3D 10 => Vista and later
*   D3D 10.1、11 => Win7 and later
*   D3D 12 => Win10 SDK

DirectX SDK下载地址:

*   [apri06](https://download.microsoft.com/download/6/4/4/6444ce7d-0703-4835-8d0d-df8d891bee5d/dxsdk_apr2006.exe) (无法下载了)
*   [mar09](https://download.microsoft.com/download/3/A/5/3A53CE87-F5C9-4CE5-92E1-5E2AF4841741/DXSDK_Mar09.exe) (无法下载了)
*   [jun10](https://download.microsoft.com/download/A/E/7/AE743F1F-632B-4809-87A9-AA1BB3458E31/DXSDK_Jun10.exe)

# 官方资料

相关技术文章 [DirectX Graphics Articles](https://docs.microsoft.com/en-us/windows/win32/direct3darticles/directx-graphics-articles-portal)，关注: 概述 [Graphics APIs in Windows](https://docs.microsoft.com/en-us/windows/win32/direct3darticles/graphics-apis-in-windows-vista) 操作系统、驱动模型与DirectX直接的关系。

总目录 [Graphics and gaming](https://docs.microsoft.com/en-us/windows/win32/graphics-and-multimedia)

[Direct3D 9 图形](https://docs.microsoft.com/zh-cn/windows/win32/direct3d9/dx9-graphics)

*   [Direct3D 9 编程指南](https://docs.microsoft.com/zh-cn/windows/win32/direct3d9/dx9-graphics-programming-guide)
*   [Direct3D 9 的参考](https://docs.microsoft.com/zh-cn/windows/win32/direct3d9/dx9-graphics-reference)
*   [D3DX (Direct3D 9)](https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dx) 一个弃用的SDK

[Direct3D 10 图形](https://docs.microsoft.com/zh-cn/windows/win32/direct3d10/d3d10-graphics)

*   [Direct3D 10 编程指南](https://docs.microsoft.com/zh-cn/windows/win32/direct3d10/d3d10-graphics-programming-guide)
*   [Direct3D 10 参考](https://docs.microsoft.com/zh-cn/windows/win32/direct3d10/d3d10-graphics-reference)

[Direct3D 11 图形](https://docs.microsoft.com/zh-cn/windows/win32/direct3d11/atoc-dx-graphics-direct3d-11)

*   [Direct3D 11 编程指南](https://docs.microsoft.com/zh-cn/windows/win32/direct3d11/dx-graphics-overviews)
*   [Direct3D 11 参考](https://docs.microsoft.com/zh-cn/windows/win32/direct3d11/atoc-d3d11-graphics-reference)
*   [How to Use Direct3D 11](https://docs.microsoft.com/en-us/windows/win32/direct3d11/how-to-use-direct3d-11) 里面有不错的主题介绍

[Direct3D 12 图形](https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/direct3d-12-graphics)

*   [Direct3D 12 编程指南](https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/directx-12-programming-guide)
*   [Direct3D 12 参考](https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/direct3d-12-reference)

以上资料都有英文版本，只需要把链接中的 `zh-cn` 修改为 `en-us` 即可。

* [DirectX-Specs](https://microsoft.github.io/DirectX-Specs/) 发布了许多 DirectX 功能的工程规范。它补充了官方 API 文档的额外详细信息，对专家开发人员来说可能很有用。

自从Win8开始，DirectX SDK包含在了Windows SDK中。[Where is the DirectX SDK?](https://docs.microsoft.com/en-us/windows/win32/directx-sdk--august-2009-)

DirectX 最新版本的各类资料：

- [DirectX Landing Page](https://devblogs.microsoft.com/directx/landing-page/) 此页面包含 Microsoft DirectX 团队已构建或正在积极开发的所有组件的最新版本的描述和链接。

# 非官方资料

- [rastertek](https://www.rastertek.com/tutindex.html) 介绍DirectX 11和OpenGL的教程。

# 性能

Intel的性能分析工具：[Intel® Graphics Performance Analyzers](https://www.intel.com/content/www/us/en/developer/tools/graphics-performance-analyzers/overview.html)


# 调试

*   Direct3D SDK 调试层技巧 : [Direct3D SDK Debug Layer Tricks](https://walbourn.github.io/direct3d-sdk-debug-layer-tricks/)
*   [Using the debug layer to debug apps](https://docs.microsoft.com/en-us/windows/win32/direct3d11/using-the-debug-layer-to-test-apps)
*   [如何调试DX程序](https://www.cnblogs.com/graphics/archive/2011/03/14/1984200.html)
*   [Enabling Direct3D Debug Information (Direct3D 9)](https://learn.microsoft.com/en-us/windows/win32/direct3d9/enabling-direct3d-debug-information)
*   DirectX Control Panel 中 DirectX 9 标签页中一些项无法使用：[Some options on DirectX control panel was disabled on Windows 8](https://stackoverflow.com/questions/16772123/some-options-on-directx-control-panel-was-disabled-on-windows-8)，原因是DirectX SDK（2010 年 6 月）中的Developer Runtime (aka D3D9D.DLL) 与Win8不兼容，需要安装checked版本，下载地址：？？

## 工具

*   [GPUView](https://docs.microsoft.com/en-us/windows-hardware/drivers/display/using-gpuview)
*   [Matt's Webcorner - GPUView](https://graphics.stanford.edu/\~mdfisher/GPUView.html)
*   [PIX on Windows](https://devblogs.microsoft.com/pix/)，[下载](https://devblogs.microsoft.com/pix/download/)，[GPU Captures](https://devblogs.microsoft.com/pix/gpu-captures/)
*   [NVIDIA PerfHUD](https://developer.nvidia.com/nvidia-perfhud) nVidia为Vista和Win7提供的一个调试工具，更高版本系统上使用的是：[CUDA Toolkit](https://developer.nvidia.com/cuda-toolkit) 的 [NVIDIA Nsight Visual Studio Edition](https://developer.nvidia.com/nsight-visual-studio-edition)

# 相关开源库

- [DirectXMath](https://learn.microsoft.com/en-us/windows/win32/dxmath/directxmath-portal) `XNAMATH` 的最新版本，DirectXMath API 为 DirectX 应用程序常见的常见线性代数和图形数学运算提供 SIMD 友好的 C++ 类型和函数。这个库微软开源并托管在了 GitHub 上：[microsoft/DirectXMath](https://github.com/Microsoft/DirectXMath)。支持 nuget 下载安装，可以在 [nuget 官网](https://www.nuget.org/packages) 上找到。
- [microsoft/DirectXShaderCompiler](https://github.com/microsoft/DirectXShaderCompiler) 包括一个编译器和相关工具，用于将高级着色器语言 (HLSL) 程序编译为 DirectX 中间语言 (DXIL) 表示形式。使用 DirectX 进行图形、游戏和计算的应用程序可以使用它来生成着色器程序。


# 开源图形编辑工具

- [blender/blender](https://github.com/blender/blender) Blender 是免费的开源 3D 创作套件。它支持整个 3D 管道建模、绑定、动画、模拟、渲染、合成、运动跟踪和视频编辑。