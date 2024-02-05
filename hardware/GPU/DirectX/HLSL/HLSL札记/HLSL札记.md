[toc]

# 基础知识

## HLSL不同版本与DirectX的对应关系

- Direct3D 9着色器可以使用shader model 1、2、3进行设计；
- Direct3D 10 着色器只能在shader model  4 上设计。
- Direct3D 11 着色器可以在shader model  5 上设计。
- Direct3D 11.3 着色器可以在shader model  5.1 上设计。
- Direct3D 12 也可以在shader model  5.1、 6 上设计。

参考官方文档： [Programming guide for HLSL](https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-pguide)

## 如何编写 HLSL

### 扩展名
- 扩展名为 .hlsl 的文件包含高级着色语言 (HLSL) 源代码。还支持较旧的 .fx 扩展名，但通常与旧版效果系统相关联。
- 扩展名为 .cso 的文件包含已编译的着色器对象。
- 扩展名为 .h 的文件是一个头文件，但在着色器代码上下文中，该头文件定义了一个保存着色器数据的字节数组。HLSL 着色器代码头的其他常见扩展名包括 .hlsli 和 .fxh。

### 编写方式

有两种：
1. 构建程序的时候编译 HLSL 为目标文件 

    ```C++
    auto vertexShaderBytecode = ReadData("SimpleVertexShader.cso");
    ComPtr<ID3D11VertexShader> vertexShader;
    DX::ThrowIfFailed(
        m_d3dDevice->CreateVertexShader(
            vertexShaderBytecode->Data,
            vertexShaderBytecode->Length,
            nullptr,
            &vertexShader
            )
    ```
2. 构建时编译到头文件

将 .hlsl 文件编译为头文件中定义的字节数组，则需要在代码中包含这些头文件。在此示例代码中，`PixelShader.hlsl` 文件的输出文件属性指定编译为 `PixelShader.h` 头文件中定义的 `g_psshader` 字节数组。

```C++
namespace
{
       include "PixelShader.h"
}
...
ComPtr<ID3D11PixelShader> m_pPixelShader;
hr = pDevice->CreatePixelShader(g_psshader, sizeof(g_psshader), nullptr, &m_pPixelShader);
```

3. 使用 `D3DCompileFromFile` 编译

