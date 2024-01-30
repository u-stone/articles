[TOC]

# DirectX版本与os系统版本的对应关系

*   DirectX 11 - 对应Win7
*   DirectX 11.1 - 对应Win8
*   DirectX 11.2 - 对应Win 8.1
*   DirectX 11.3/11/4 （新增特性同样也被DirectX 12支持） - 对应 Win 10？

# 调试

*   对于带有 Windows 7 (KB2670838) 或 Windows 8.x 平台更新的 Windows 7，要创建支持调试层的设备，请安装适用于 Windows 8.x 的 Windows 软件开发工具包 (SDK) 以获取 D3D11\_1SDKLayers.dll。
*   对于 Windows 10，要创建支持调试层的设备，请启用“图形工具”可选功能。转到设置面板，在系统、应用程序和功能下、管理可选功能、添加功能，然后查找“图形工具”。
*   PIX 可用来调试与分析着色器运行状态，[官网](https://devblogs.microsoft.com/pix/)，以及[文档](https://devblogs.microsoft.com/pix/documentation/)
*   DirectX功能查看器：DirectX Caps Viewer 查看硬件对D3D，DirectDraw，DirectSound，DirectInpu的支持。位于DirectX SDK中。
*   DxDiag 用于诊断DX的组件是否正常
*   Direct Texture Tool Dx9的纹理压缩工具
*   DirectX Error Lookup 错误查找工具
*   XACT 微软开发的一款音频编辑软件
*   [baldurk/renderdoc](https://github.com/baldurk/renderdoc) RenderDoc 是一种基于帧捕获的图形调试器，目前可用于 Windows、Linux、Android 和 Nintendo Switch™ 上的 Vulkan、D3D11、D3D12、OpenGL 和 OpenGL ES 开发，[官网](https://renderdoc.org/)：https://renderdoc.org/

# 对硬件的支持&兼容

下图显示 Direct3D 11 如何支持新的和现有的硬件：

![image](https://docs.microsoft.com/en-us/windows/win32/direct3d11/images/d3d11-on-downlevel-hardware.png)

# [功能级别](https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-devices-downlevel-intro)

为了处理新机器和现有机器中视频卡的多样性，Microsoft Direct3D 11 引入了功能级别的概念。功能级别是一组定义明确的 GPU 功能。例如，9\_1 功能级别实现了在 Microsoft Direct3D 9 中实现的功能，它公开了着色器模型ps\_2\_x和vs\_2\_x的功能

# 可选功能检查

调用 [ID3D11Device::CheckFeatureSupport](https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-checkfeaturesupport)，更多信息参考 [Using Direct3D 11 feature data to supplement Direct3D feature levels](https://docs.microsoft.com/en-us/windows/win32/direct3d11/using-direct3d-optional-features-to-supplement-direct3d-feature-levels)

# 资源

DirectX 11的资源都是通过[ID3D11Device interface](https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nn-d3d11-id3d11device)的接口创建，通过[ID3D11DeviceContext interface](https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nn-d3d11-id3d11devicecontext)的接口绑定到管道，最后由Release方法释放资源。

## DirectDraw Surface file format (.dds)

参考 [DDS](https://docs.microsoft.com/en-us/windows/win32/direct3ddds/dx-graphics-dds)纹理存储格式，以及数据压缩技术[Block Compression (Direct3D 10)](https://docs.microsoft.com/en-us/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-block-compression)。

微软的开源项目 [microsoft/DirectXTK](https://github.com/Microsoft/DirectXTK)(开发工具包toolkit) 和 [microsoft/DirectXTex](https://github.com/Microsoft/DirectXTex)(纹理处理库) 都支持 `dds` 格式。

# [浮点规则 (Direct3D 11)](https://docs.microsoft.com/en-us/windows/win32/direct3d11/floating-point-rules)

介绍了10、11、16、32、64位浮点数的规则

# 纹理平铺

参考 [Tiled resources](https://docs.microsoft.com/en-us/windows/win32/direct3d11/tiled-resources)。

# 图形处理流程

![Direct3D 11 可编程管道](https://docs.microsoft.com/en-us/windows/win32/direct3d11/images/d3d11-pipeline-stages.jpg)

# DirectX 11 的重要功能之一： [多线程](https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-render-multi-thread)

由于多核系统变得越来越可用，Direct3D 11 改进了其多线程支持，以实现多个 CPU 线程和 D3D11 图形 API 之间的高效交互。

# DirectX 11 的重要功能之二：[DirectCompute 技术](https://docs.microsoft.com/en-us/windows/win32/direct3d11/direct3d-11-advanced-stages-compute-shader)

这个技术可将 Microsoft Direct3D 11 扩展到图形编程之外。

# [渲染](https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-render)

[立即和延迟渲染](https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-render-multi-thread-render)

*   立即渲染是指从设备调用渲染 API 或命令，将命令在缓冲区中排队以在 GPU 上执行。使用即时上下文来渲染、设置管道状态和回放命令列表。
*   延迟渲染将图形命令记录在命令缓冲区中，以便它们可以在其他时间播放。一般与 [Multiple-Pass Rendering](https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-render-multipass) 联动。具体使用参考 [如何：记录命令列表](https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-render-multi-thread-command-list-record) 和 [如何：回放命令列表](https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-render-multi-thread-command-list-play)。**命令列表必须由延迟上下文记录，但它只能在即时上下文中回放。**

# [HLSL语法](https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl)

# Direct2D与Direct3D交互

[Direct2D and Direct3D Interoperability Overview](https://docs.microsoft.com/en-us/windows/win32/direct2d/direct2d-and-direct3d-interoperation-overview)

2D 和 3D的交互，目前不推荐使用[Direct Draw](https://docs.microsoft.com/en-us/windows/win32/directdraw/directdraw)，Windows 7之后推荐使用 [Direct 2D](https://docs.microsoft.com/en-us/windows/win32/direct2d/direct2d-portal)。

关于加载本地图片文件，参考 [Render a BMP, JPEG, or PNG Image with DirectX?](https://stackoverflow.com/questions/5957624/render-a-bmp-jpeg-or-png-image-with-directx)，不过目前都推荐使用微软的 [microsoft/DirectXTK](https://github.com/Microsoft/DirectXTK)(开发工具包toolkit) 和 [microsoft/DirectXTex](https://github.com/Microsoft/DirectXTex)(纹理处理库) 。

另外，DirectXTex 库支持绘制文本。参考 [Drawing text](https://github.com/microsoft/DirectXTK/wiki/Drawing-text)。如果是DirectX 9，那么要使用 [ID3DXFont::DrawText method](https://docs.microsoft.com/en-us/windows/win32/direct3d9/id3dxfont--drawtext)

# Windows Imaging Component ([WIC](https://docs.microsoft.com/en-us/windows/win32/wic/-wic-lh))

Windows 映像组件 (WIC) 是一个可扩展平台，它为数字图像提供低级 API。

# 示例代码

可以在安装 [DirectX SDK](https://download.microsoft.com/download/A/E/7/AE743F1F-632B-4809-87A9-AA1BB3458E31/DXSDK_Jun10.exe) 之后，在 `C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Samples\C++\Direct3D11` 下找到。这里面还有一个教程。

# 设备枚举，以及设备支持的模式

一台电脑可能支持多个display adapter， 一个display adapter可能支持多种颜色format，而一种颜色format可能对应多种模式（分辨率、刷新率、显示角度）。所以要获取机器运行环境的全部模式一共是分为三层：

*   display adapter: IDXGIAdapter
*   adapter output：（显示器是一种adapter output），IDXGIOutput
*   具体的显示模式：DXGI\_MODE\_DESC 描述

具体代码可以参考：

    std::vector<IDXGIAdapter*> Wrapper::EnumerateAdapterAndMore()
    {
        IDXGIAdapter* pAdapter;
        std::vector<IDXGIAdapter*> vAdapters;
        IDXGIFactory* pFactory = NULL;

        // Create a DXGIFactory object.
        if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory))) {
            return vAdapters;
        }

        for (UINT i = 0;
             pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND;
             ++i) {
            vAdapters.push_back(pAdapter);

            IDXGIOutput* pOutput = NULL;
            HRESULT hr = pAdapter->EnumOutputs(0, &pOutput);
            if (FAILED(hr)) {
              continue;
            }
            DXGI_ADAPTER_DESC desc;
            hr = pAdapter->GetDesc(&desc);

            UINT numModes = 0;

            for (int index = 0; index < DXGI_FORMAT_V408; ++index) {
              DXGI_FORMAT format = static_cast<DXGI_FORMAT>(index);

              // Get the number of elements
              hr = pOutput->GetDisplayModeList(format, 0, &numModes, NULL);
              if (FAILED(hr) || numModes == 0) {
                continue;
              }
              std::unique_ptr<DXGI_MODE_DESC[]> displayModes(new DXGI_MODE_DESC[numModes]);

              // Get the list
              hr = pOutput->GetDisplayModeList(format, 0, &numModes, displayModes.get());
              if (SUCCEEDED(hr)) {
                for (int index = 0; index < numModes; ++index) {
                  auto mode = displayModes.get()[index];
                  mode.Height = mode.Height;
                }
              }
            }
        }

        if (pFactory) {
            pFactory->Release();
        }

        return vAdapters;
    }

# CPU和GPU之间纹理拷贝的最佳实践

使用 `ID3D11Device::CreateTexture2D` 创建纹理的时候，可以给 `D3D11_TEXTURE2D_DESC` 中的 `Usage` 指定不同的参数。

    D3D11_TEXTURE2D_DESC texDescStage;
    texDescStage.ArraySize = texDescStage.MipLevels = 1;
    texDescStage.Height = height;
    texDescStage.Width = width;
    texDescStage.Format = DXGI_FORMAT_R8_UNORM;
    texDescStage.SampleDesc.Quality = 0;
    texDescStage.SampleDesc.Count = 1;
    texDescStage.BindFlags = 0;
    texDescStage.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    texDescStage.MiscFlags = 0;
    texDescStage.Usage = D3D11_USAGE_STAGING;

    ID3D11Device* device; // ...
    device->CreateTexture2D(&texDescStage, NULL, &texStage);

比如上面创建的就是一个staging纹理，根据官方API描述：

*   `D3D11_USAGE_DEFAULT` 仅限GPU读写
*   `D3D11_USAGE_DYNAMIC` 适用于CPU生成数据，或者说CPU作为数据的生产者，GPU作为数据的消费者
*   `D3D11_USAGE_IMMUTABLE` 适用于数据直接载入GPU，不经过系统内存的方式
*   `D3D11_USAGE_STAGING` 这类纹理可以供GPU和CPU读写，但是只允许CPU与GPU之间通过特定的API（ID3D11DeviceContext::CopySubresourceRegion和ID3D11DeviceContext::CopyResource）复制数据，不允许CPU直接读写。

其中 `D3D11_USAGE_DYNAMIC` 和 `D3D11_USAGE_STAGING` + `D3D11_USAGE_DEFAULT` 的实现值得拿出来一说。

如果是CPU生产数据，交给GPU处理，那么两种方式都可以实现。

*   使用 `D3D11_USAGE_DYNAMIC` 的话，适用于频繁地将内存中的数据拷贝到显存中的场景。
*   使用 `D3D11_USAGE_STAGING` + `D3D11_USAGE_DEFAULT` 的话，可以实现系统内存和显存数据的双向传输。

纹理资源的传输可以由下面3类函数实现：

*   Map / Unmap 方法适用于那种标记为CPU读/写的资源，可以通过这个方法映射到系统内存，进一步读/写。
*   UpdateSubresource 方法只允许写，不允许读，如果是要对一个很大的资源更新很小的一个区域，那么使用这个方法会有更好的性能。
*   CopyResource、CopySubresourceRegion 拷贝资源，可用于在GPU资源和CPU资源直接传输数据。由于`D3D11_USAGE_DEFAULT`类型的资源不能被CPU直接访问，所以可以用这套函数拷贝到  `D3D11_USAGE_STAGING`类型的资源上，再让CPU通过Map/Unmap访问。

参考：

*   MSDN
    *   [DXGI\_ERROR](https://docs.microsoft.com/en-us/windows/win32/direct3ddxgi/dxgi-error)
    *   [Direct3D 11 Return Codes](https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d11-graphics-reference-returnvalues)
    *   [ID3D11Device::CreateTexture2D method (d3d11.h)](https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createtexture2d)
    *   [ID3D11DeviceContext::UpdateSubresource method (d3d11.h)](https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-updatesubresource)
    *   [D3D11\_USAGE enumeration (d3d11.h)](https://docs.microsoft.com/zh-cn/windows/win32/api/d3d11/ne-d3d11-d3d11_usage) 介绍了各种 `Usage` 的资源在CPU和GPU读写上的差异。
    *   [How to: Use dynamic resources](https://docs.microsoft.com/en-us/windows/win32/direct3d11/how-to--use-dynamic-resources)
    *   [Resource Management Best Practices](https://docs.microsoft.com/en-us/windows/win32/dxtecharts/resource-management-best-practices) 比较老的一个资料，很多名词在DirectX 11中已经改变，可做参考。
*   [DirectX11--深入理解与使用2D纹理资源](https://www.cnblogs.com/X-Jun/p/10262524.html)
*   StackOverflow
    *   [D3D11\_USAGE\_STAGING, what kind of GPU/CPU memory is used?](https://stackoverflow.com/questions/50396189/d3d11-usage-staging-what-kind-of-gpu-cpu-memory-is-used)
    *   [ID3D11DeviceContext::Map slow performance](https://stackoverflow.com/questions/40808759/id3d11devicecontextmap-slow-performance)
*   gamedev.net上的一个问题，[\[D3D11\] What is the most performant way to partially but frequently update a large vertex buffer?](https://www.gamedev.net/forums/topic/711216-d3d11-what-is-the-most-performant-way-to-partially-but-frequently-update-a-large-vertex-buffer/5443575/)， 提问者测试了多种更新数据的方式，提供了性能对比，虽然是顶点数组，但是纹理的操作可以参考这个。

# Present 渲染

参考：

- MSDN
    - [DXGI flip model](https://docs.microsoft.com/en-us/windows/win32/direct3ddxgi/dxgi-flip-model)

# AIGC 问答

- Q: 一个DirectX入门的开发者，如何学习DirectX的Effect技术？
- A:
  ```
  作为一个DirectX入门的开发者，学习DirectX的Effect技术可以按照以下步骤进行：

  1. 了解基本的DirectX知识：首先，您需要掌握基本的DirectX知识，包括图形渲染管线、顶点和像素着色器等。可以通过阅读相关的教程或文档来了解这些基础概念。

  2. 学习Effect框架：Effect是DirectX提供的一种简化着色器编程的框架。您可以学习Effect框架的基本概念和使用方法，包括Effect文件的语法和结构，以及如何在应用程序中加载和使用Effect文件。

  3. 编写简单的Effect文件：从简单的例子开始，编写一些基本的Effect文件。可以从渲染一个简单的三角形开始，逐步增加效果和复杂度，以加深对Effect技术的理解。

  4. 实践和实验：通过实践和实验，您可以进一步熟悉Effect技术的应用。尝试在应用程序中使用Effect文件来实现不同的渲染效果，如光照、阴影、纹理映射等。

  5. 学习和参考示例代码：查阅DirectX的官方文档和示例代码，以及其他相关的教程和书籍。这些资源可以帮助您深入了解Effect技术的各个方面，并提供实际的代码示例供参考。

  6. 加入开发社区：加入DirectX开发者的社区或论坛，与其他开发者交流经验和技术。这样可以获得更多的学习资源和支持，同时也可以分享自己的经验和成果。

  总之，学习DirectX的Effect技术需要一定的时间和实践，但通过持续的学习和实践，您将逐渐掌握这一技术，并能够在应用程序中灵活地使用它。祝您学习顺利！
  ```
- Q: effect与hlsl有什么关系?
- A: 
  ```
  Effect与HLSL（High-Level Shading Language）是密切相关的，下面是它们之间的关系：

  Effect是一种用于简化HLSL编程的框架。它提供了一种结构化的方式来定义和管理着色器及其相关资源，如纹理、常量缓冲等。Effect文件使用HLSL作为其内部的着色器语言。

  HLSL是一种用于编写着色器的高级语言。它与C++相似，并具有一些额外的特性和语法来支持图形渲染管线中的顶点和像素着色器的编写。HLSL代码需要编译为GPU可以执行的字节码，这通常通过DirectX编译器实现。

  在Effect框架中，您可以使用HLSL编写着色器代码，并将其嵌入到Effect文件中。Effect文件中还可以包含一些额外的语法和语义来描述着色器的参数、纹理、渲染状态等信息。通过Effect文件，您可以更方便地管理和使用着色器及其相关资源。

  总而言之，Effect是一种在HLSL基础上构建的框架，它简化了HLSL着色器的编程和管理，使得开发者可以更便捷地定义和使用着色器及其相关资源。HLSL是用于编写着色器代码的语言，Effect文件使用HLSL作为其内部的着色器语言。
  ```

# 资料

- [DirectX 11 龙书中文版译者网站](https://enjoyphysics.cn/Soft/Program)
- B 站教程：[C++ 3D DirectX Programming(DirectX 11)](https://www.bilibili.com/video/BV17J411J7MB/?spm_id_from=333.999.0.0&vd_source=adf6eac1e33345717d8cd94de0070db7)，视频对应的Wiki：[Hardware 3D Series (C++ DirectX Graphics)](https://wiki.planetchili.net/index.php/Hardware_3D_Series_(C%2B%2B_DirectX_Graphics))
- [Windows Advanced Rasterization Platform (WARP) Guide](https://learn.microsoft.com/en-us/windows/win32/direct3darticles/directx-warp)
- Clayman 的 [游戏程序员养成计划 (更新2013.3.19)](https://www.cnblogs.com/clayman/archive/2009/05/17/1459001.html)
- [zhygmsz/book](https://github.com/zhygmsz/book/tree/master) 电子版优秀书籍的一个集合