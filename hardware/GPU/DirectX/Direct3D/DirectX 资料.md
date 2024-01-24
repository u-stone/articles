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

# MSDN官方资料

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

自从Win8开始，DirectX SDK包含在了Windows SDK中。[Where is the DirectX SDK?](https://docs.microsoft.com/en-us/windows/win32/directx-sdk--august-2009-)

# 工具

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

