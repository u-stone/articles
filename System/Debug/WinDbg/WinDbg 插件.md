

# 编写 WinDbg 插件

这里有一个教程：[Debug Tutorial](https://bbs.kanxue.com/thread-103667.htm)，其中第四篇是教你如何编写一个 WinDbg 扩展插件。翻译版本可以参考：[编写 windbg 扩展](https://blog.csdn.net/superliuxing/article/details/19489979)。

官方资料参考 [Writing New Debugger Extensions](https://learn.microsoft.com/en-us/windows-hardware/drivers/debuggercmds/writing-new-debugger-extensions)。

# 几个有名的 WinDbg 插件

偶然发现这里有一个合集 [anhkgg/awesome-windbg-extensions](https://github.com/anhkgg/awesome-windbg-extensions).

下面介绍部分我试用过的：

## skywing 的 sdbgext

skywing 的插件 [sdbgext](http://www.nynaeve.net/?p=94)，不过，已经很久没更新了。我这里保存了一下[这个文件](./files/sdbgext.zip)。我测试了一下，在我的 WinDbg x64上加载失败了，只能放在 x86 下使用，具体的使用方法是：

1. 解压出来之后放在 x86 版本的 WinDbg 目录下的 winext 文件夹中 (C:\Program Files (x86)\Windows Kits\10\Debuggers\x86\winext)。
2. 打开 WinDbg 的 x86 版本，执行命令：`.load sdbgext`，加载成功之后，执行 `!sdbgext.usermsg` 如果看到有提示 `Usage: !usermsg <address>` 那就加载成功了。或者执行命令 `.chain` 可以看到当前加载的插件，如果 `sdbgext` 也有列出来那就是加载成功了。

## hwnd

可以在这里下载到源码 [blabberer/hwnd](https://github.com/blabberer/hwnd)，不过直接编译的话会提示找不到文件 `modwinternl.h`。看了一下代码里面的 `diffforwinternl.txt`，可以发现作者在这里故意没加，因为这个文件是根据系统中的 "c:\Program Files (x86)\Windows Kits\10\Include\10.0.17763.0\um\winternl.h" 文件做了一点点修改。我就直接放在[这里](./files/modwinternl.h)了，把这个文件保存在下载好的代码目录就可以编译通过了。自己编译的话，可以编译 x64 和 x86 两个版本的 Release 包。

按照上面提到的步骤加载 hwnd，之后直接运行 `!hwnd`，会打印当前系统中全部窗口，选择其中一个窗口句柄 (00020018)，可以看到具体信息，比如：

```shell
0:006> !hwnd 00020018
Window    00020018
Name      Default IME
Class     IME
WndProc   00000000
Style     WS_OVERLAPPED 
ExStyle   WS_EX_LEFT WS_EX_LTRREADING WS_EX_RIGHTSCROLLBAR 
HInstance 8fb00000
ParentWnd 0002001c
Id        00000000
UserData  00000000
Unicode   TRUE
ThreadId  000017b8
ProcessId 000017b4
```

当然还要重点提一下微软的 [microsoft/WinDbg-Samples](https://github.com/Microsoft/WinDbg-Samples)。