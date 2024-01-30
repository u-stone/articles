[TOC]

# 错误处理
## 打印 HRESULT 错误信息

可以使用Windows平台提供的 [`_com_error`](https://learn.microsoft.com/en-us/cpp/cpp/com-error-com-error) 封装类：

```C++
#include <comdef.h>

_com_error err(hr);
LPCTSTR errMsg = err.ErrorMessage();
```

## DirectX 11 打开调试层

参考 [Using the debug layer to debug apps](https://learn.microsoft.com/en-us/windows/win32/direct3d11/using-the-debug-layer-to-test-apps)

要创建支持调试层的设备，必须安装 DirectX SDK（以获取 D3D11SDKLayers.dll），然后在调用 `D3D11CreateDevice` 函数或 `D3D11CreateDeviceAndSwapChain` 函数时指定 `D3D11_CREATE_DEVICE_DEBUG` 标志。

```C++
UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
        // 重点在这里：创建设备的时候，指定 D3D11_CREATE_DEVICE_DEBUG 标记
        // If the project is in a debug build, enable the debug layer.
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        // Define the ordering of feature levels that Direct3D attempts to create.
        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_1
        };

        ComPtr<ID3D11Device> d3dDevice;
        ComPtr<ID3D11DeviceContext> d3dDeviceContext;
        DX::ThrowIfFailed(
            D3D11CreateDevice(
                nullptr,                    // specify nullptr to use the default adapter
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,                    // specify nullptr because D3D_DRIVER_TYPE_HARDWARE 
                                            // indicates that this function uses hardware
                creationFlags,              // optionally set debug and Direct2D compatibility flags
                featureLevels,
                ARRAYSIZE(featureLevels),
                D3D11_SDK_VERSION,          // always set this to D3D11_SDK_VERSION
                &d3dDevice,
                nullptr,
                &d3dDeviceContext
                )
            );
```

如果出错就会在调试信息中打印类似下面这样的信息：


> DXGI WARNING: IDXGIFactory::CreateSwapChain: Blt-model swap effects (DXGI_SWAP_EFFECT_DISCARD and DXGI_SWAP_EFFECT_SEQUENTIAL) are legacy swap effects that are predominantly superceded by their flip-model counterparts (DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL and DXGI_SWAP_EFFECT_FLIP_DISCARD). Please consider updating your application to leverage flip-model swap effects to benefit from modern presentation enhancements. More information is available at http://aka.ms/dxgiflipmodel. [ MISCELLANEOUS WARNING #294: ]
> DXGI ERROR: IDXGIFactory::CreateSwapChain: DXGI_SWAP_CHAIN_FLAG_NONPREROTATED can only be specified in DXGI_SWAP_CHAIN_DESC::Flags, when DXGI_SWAP_CHAIN_DESC::Windowed is FALSE, since the flag only affects fullscreen swap chains. [ MISCELLANEOUS ERROR #34: ]
> Exception thrown at 0x00007FFAB28ACF19 in Dx11.exe: Microsoft C++ exception: _com_error at memory location 0x0000005895EFEAE0.

对比上面两种打印错误的方式，后一种方式对于 DirectX 来说打印的错误信息更加全面。前一种不止可以打印 DirectX 的错误信息。


