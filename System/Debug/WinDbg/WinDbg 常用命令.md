
[TOC]

# 用户态

## 格式

*   n 打印当前数字进制表示
*   n 10 切换到十进制
*   n 16 切换到十六进制
*   .formats Number 显示Number的各种表示方式

## 异常

*   .exr Address 打印异常记录
*   .exr -1 打印最近一次异常的记录
*   .ecxr 打印与当前异常有关的上下文
*   .cxr \[*Option*] \[*Address*] 打印保存在目标地址中的上下文记录
*   !analyze -v 执行自动化分析，检查异常。成功执行的话会展示很多信息：异常类型，异常代码，异常发生的系统环境，进程名，发生异常的线程、模块，以及异常线程的调用堆栈等。参考 [Using the !analyze Extension](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/using-the--analyze-extension)。
*   .lastevent 打印当前线程最后一次发生的异常或者事件
*   !error Value \[Flags] 翻译指定错误码
*   !locks 显示与当前进程关联的所有临界区，以及拥有线程的 ID 和每个临界区的锁计数。如果临界区的锁定计数为零，则它不会被锁定。检查死锁参考 [Debugging a deadlock](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/debugging-a-deadlock)
*   !cs 显示整个关键帧树，或者某一个关键帧的详细信息 [Displaying a Critical Section](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/displaying-a-critical-section)
*   !critsec *Address* 打印指定位置的关键帧（RTL\_CRITICAL\_SECTION ）信息

### 实操：

*   [Recognizing different types of exception objects that Windows platform libraries can throw](https://devblogs.microsoft.com/oldnewthing/20200821-00/?p=104112)
*   [[WinDBG] C++ EH Exception (0xE06D7363)](https://medium.com/renee0918/windbg-c-eh-exception-0xe06d7363-e9f8808f1eab)

## 内存

*   s 搜索内存：`s -a 0012ff40 L20 "Hello" `
*   !address 列出整个用户态空间所有区域的，并带有模块的位置：x64的用户态空间就是\[00000000 00000000, 000007ff fffdf300]
*   !address Address 查看指定内存地址信息
*   !address -summary 内存概要信息；如果是调试dump，这个命令需要全量dump，minidump不行
*   .writemem FileName Range 将内存写入文件
*   e\* 写内存指令
*   !vprot 扩展命令显示虚拟内存保护信息。可以用于活动调试和dump文件调试。
*   !vadump 这个会显示所有的虚拟内存区域和它的保护属性

## 进程 | 线程

*   \| 显示当前进程信息
*   \~ 显示当前线程信息
*   \~. 当前线程信息
*   \~\* 所有线程信息
*   \~1s 切换到序号为1的线程
*   \~\~\[0x2b40]s 切换到线程ID为 `0x2b40` 的线程
*   \~n 线程挂起计数+1. 为0的时候线程正常运行，调试器附加的时候+1，调试器分离进程时-1.
*   \~m 线程挂起计数-1
*   \~f 冻结线程，默认情况下线程状态未知，调试器启动的进程，在执行冻结线程之后不会执行，调试器分离进程之后，所有线程解冻
*   \~u 解冻指定线程
*   .ttime 打印当前线程的运行时间，如果是调试dump文件，那么需要生成dump的时候 `.dump` 的命令选项中设置上 `/t`，不过 `.dump /mf[/ma]` 中带有 `/t` 选项。
*   ~e cmd、~\*e cmd 指定线程或者所有线程执行一个命令cmd
*   !teb 打印当前线程环境，堆栈信息也包含在这个信息块里面
*   !peb 打印当前进程的信息，其中有模块加载的信息，程序启动时的环境变量，设置的启动参数
*   dt nt!\_ethread  打印 ntdll!\_ETHREAD 结构
*   dt nt!\_kthread  打印 ntdll!\_KTHREAD 结构
*   `dt _TEB -y Last` 这个意思是搜索结构`_TEB`中名字开头是`Last`的字段
*   .tlist 列举当前系统上运行的进程
*   \~e CMD 在指定线程执行命令
*   \~\*e CMD 在所有线程执行命令
*   !gle 得到当前线程的最后一个错误信息，具体是读取当前线程上下文的一个寄存器的值。记住有些API只修改LastErroValue，而不修改LastStatusValue，会导致前者显示成功，但后者有一个错误状态值。
*   `~*e ? @$tid; !gle` 针对每一个线程查看其最后一个错误是什么。

## 模块

*   lm 列举所有模块
*   lmsm 按照模块名排序
*   lmDvmSymbol 列出Symbol的详细信息
*   .load DLLName 加载DLLName(要么是完整路径，要么直接保存到windbg下的winext文件夹下，只需要使用dll文件名)进调试器
*   .loadby DLLName ModuleName 从指定的模块地址同目录加载Dll。这个DLLName不需要是完整路径，WinDbg会找。
*   !dh 显示指定镜像文件的头部信息

## 符号

*   .symfix\[+] \[LocalSymbolCache] 设置符号的本地缓存路径
*   .sympath+\[Path\[; ...]] 添加符号路径
*   .sympath path 直接赋值符号路径
    微软符号服务器：srv*E:\Symbols*<http://msdl.microsoft.com/download/symbols>
    AMD符号服务器：srv*E:\Symbols*<https://download.amd.com/dir/bin>
    （参考文档：<https://gpuopen.com/learn/amd-driver-symbol-server/）>
    nVidia：srv*E:\Symbols*<https://driver-symbols.nvidia.com/>\
    (NV binaries: <https://developer.nvidia.com/nvidia-driver-symbol-server>)
    Intel 符号服务器： srv*E:\Symbols*<https://software.intel.com/sites/downloads/symbols/>
*   .srcpath\[+] \[Directory \[; ...]]  打印或者设置源代码路径
*   ld ModuleName \[/f FileName] 加载指定模块的符号
*   .reload 重新加载符号，以及从设置的符号服务器上下载符号文件
*   !sym noisy  激活符号加载过程
*   !sym quiet  关闭符号加载过程
*   x 检查符号
*   x symbol 搜索符号的调用者
*   x \*!\* 可以打印所有module的符号
*   x /2 symbol 打印符号对应名称和地址
*   .exepath 查看可执行文件路径
*   !findstack sym 在堆栈中搜索符号sym
*   .load wow64exts  加载wow64exts.dll模块
*   .unload wow64exts  卸载wow64exts.dll模块
*   .effmach  查看当前调试mode：x86、x64等
*   .effmach x86/amd64 切换到x86/x64栈环境  注：需要先执行.load wow64exts来加载wow64exts.dll模块
*   .effmach .  切换到目标机器的原生处理器模式
*

## 打印

*   dx 打印调试对象模型表达式
*   dx -r1 ((WindowsProject1!webrtc::TrackerWnd \*)0x14398f0) 将对应的地址转为指定类型对象
*   dx -r1 (\*((WindowsProject1!webrtc::TrackerWnd \*)0x14398f0)).event\_quit\_ 打印（地址强制转换之后）对象的成员
*   dv -v 打印局部变量
*   dt var 打印变量var
*   dt nt!\_kthread 打印指定类型原型，比如有：ntdll!\_TEB, ntdll!\_PEB, ntdll!\_HEAP\_ENTRY
*   ds dS 打印字符串
*   d\[abcdDfpquw] 按照指定格式打印内存地址
*   .fnret 打印函数的返回值，不过要求有调试模块的私有符号，参考 [How to display the address of the function in WinDBG for .fnret command?](https://stackoverflow.com/questions/68319533/how-to-display-the-address-of-the-function-in-windbg-for-fnret-command)，有人推荐使用伪寄存器 `$retreg`，参考 [How to get return value from a function in windbg?](https://stackoverflow.com/questions/1025804/how-to-get-return-value-from-a-function-in-windbg)。
*   dds, dps, dqs 显示给定范围内存中的内容
*   r 打印当前寄存器，也可以修改寄存器值
*   .printf 打印输出，可以放在windbg命令脚本里面，比如 `bp 0032ff40 "r $t0=$t0+1;.printf \"function exectutes:%dtimes, size.x:%d size.y:%d\", $t0,poi(esp+8),poi(esp+c);.echo;g"`

## 断点

*   bp MAGNIFICATION!CMilMagnifier::FilterTextureD3D9 立即生效的断点
*   bu MAGNIFICATION!CMilMagnifier::FilterTextureD3D9 延迟断点，如果一个模块还没有加载进来使用这类断点
*   bm UnityPlayer!Baselib\_SystemSemaphore\_Acquire ".printf "`$tid = %d\\n\",@$`tid; gc"  设置断点，命中时打印线程id，并继续

## 调试控制

*   windbg -y SymbolPath -i ImagePath -z DumpFileName 打开dump
*   .opendump
*   .kill 终止调试
*   q 退出调试
*   qd 退出并分离
*   .detach 如果您正在调试多个目标，此命令将与当前目标分离并继续与剩余目标进行调试会话。
*   [Remote Debugging using WinDbg](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/remote-debugging-using-windbg)，以及[Remote Debugging](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/remote-debugging)
*   p 单步调试
*   t (trace) 跟踪指令/代码的执行，并打印结果
*   .call 调用函数
*   sx, sxd, sxe, sxi, sxn, sxr, sx- (Set Exceptions) 控制当被调试进程发送中断，或者有其他事件发生时调试器的行为，比如一般使用调试器加载一个app启动调试，首次断下来在`ntdll!LdrpInitialize`，如果发现正在被调试，那么就会调用`ntdll!LdrpDoDebuggerBreak`通知调试器；但是可以有更早的时机，比如进程创建 `sxe cpr`，或者exe模块加载事件 `sxe ld`。更多内容参考 [Controlling Exceptions and Events](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/controlling-exceptions-and-events)
*   e 修改汇编命令，下面是一个例子：
> ```asm
> 0:000> uf match_device  // 先看一下这个函数的汇编命令
> dshow_cap!match_device [e:\practice\will.practice\dshow_cap\dshow_cap.cpp @ 241]:
>   241 00007ff7`e7456d50 48894c2408      mov     qword ptr [rsp+8],rcx  // 241 是对应源代码的行数
>   241 00007ff7`e7456d55 55              push    rbp
>   241 00007ff7`e7456d56 57              push    rdi
>   241 00007ff7`e7456d57 4881ec28010000  sub     rsp,128h
>   241 00007ff7`e7456d5e 488d6c2420      lea     rbp,[rsp+20h]
>   241 00007ff7`e7456d63 488bfc          mov     rdi,rsp
>   241 00007ff7`e7456d66 b94a000000      mov     ecx,4Ah
>   241 00007ff7`e7456d6b b8cccccccc      mov     eax,0CCCCCCCCh
>   241 00007ff7`e7456d70 f3ab            rep stos dword ptr [rdi]
>   241 00007ff7`e7456d72 488b8c2448010000 mov     rcx,qword ptr [rsp+148h]
>   241 00007ff7`e7456d7a 48c785e8000000feffffff mov qword ptr [rbp+0E8h],0FFFFFFFFFFFFFFFEh
> 15732480 00007ff7`e7456d85 488d0dcfd20100  lea     rcx,[dshow_cap!_NULL_IMPORT_DESCRIPTOR <PERF> (dshow_cap+0x3405b) (00007ff7`e747405b)]
> 15732480 00007ff7`e7456d8c e8b4a3ffff      call    dshow_cap!ILT+320(__CheckForDebuggerJustMyCode) (00007ff7`e7451145)
>   249 00007ff7`e7456d91 488d15f8bb0000  lea     rdx,[dshow_cap!`string' (00007ff7`e7462990)]
>   249 00007ff7`e7456d98 488b8d20010000  mov     rcx,qword ptr [rbp+120h]
>   249 00007ff7`e7456d9f e8bea4ffff      call    dshow_cap!ILT+605(??$?8_WU?$char_traits_WstdV?$allocator_W (00007ff7`e7451262)
>   249 00007ff7`e7456da4 8885c4000000    mov     byte ptr [rbp+0C4h],al
>   249 00007ff7`e7456daa 488b8d20010000  mov     rcx,qword ptr [rbp+120h]
>   249 00007ff7`e7456db1 e880a8ffff      call    dshow_cap!ILT+1585(??1?$basic_string_WU?$char_traits_WstdV?$allocator_W (00007ff7`e7451636)
>   249 00007ff7`e7456db6 0fb685c4000000  movzx   eax,byte ptr [rbp+0C4h]
>   251 00007ff7`e7456dbd 488da508010000  lea     rsp,[rbp+108h]
>   251 00007ff7`e7456dc4 5f              pop     rdi
>   251 00007ff7`e7456dc5 5d              pop     rbp
>   251 00007ff7`e7456dc6 c3              ret
> 0:000> eb 00007ff7`e7456d57 5f; eb 00007ff7`e7456d58 5d; eb 00007ff7`e7456d59 c3 // 找到要修改汇编命令的地址，然后通过e的各种变种命令执行对应地址的命令修改
> 0:000> uf match_device // 再看一下修改完之后的函数
> dshow_cap!match_device [e:\practice\will.practice\dshow_cap\dshow_cap.cpp @ 241]:
>   241 00007ff7`e7456d50 48894c2408      mov     qword ptr [rsp+8],rcx
>   241 00007ff7`e7456d55 55              push    rbp
>   241 00007ff7`e7456d56 57              push    rdi
>   241 00007ff7`e7456d57 5f              pop     rdi
>   241 00007ff7`e7456d58 5d              pop     rbp
>   241 00007ff7`e7456d59 c3              ret
> ```
*   tt 执行到下一个return指令

## 栈

*   k 查看调用堆栈
*   kPL 查看调用堆栈并显示已知函数的原型
*   dx Debugger.Sessions\[0].Processes\[9664].Threads\[16564].Stack.Frames\[6].SwitchTo();dv /t /v 打印进程9664的线程16564的栈的6号栈帧，并打印局部变量。

## 表达式

*   [MASM Numbers and Operators](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/masm-numbers-and-operators)

*   [C++ numbers and operators](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/c---numbers-and-operators)

*   .expr 选择表达式计算器

*   `.expr /s masm` 切换默认计算器为 `masm`

*   `.expr /s c++ ` 切换默认计算器为 `C++`

*   可以使用 @@c++( ... ) 或者 @@masm( ... ) 指定选择表达式计算器

*   使用脚本的时候，可用于 `.if/.else/.elif` 的判断条件。

*   poi

## 内核对象

*   !handle 输出内核对象信息，包括了很多种类型：
    *   None
    *   Event
    *   Section
    *   File
    *   Directory
    *   Mutant
    *   WindowStation
    *   Semaphore
    *   Key
    *   Thread
    *   Desktop
    *   IoCompletion
    *   Timer
    *   TpWorkerFactory
    *   ALPC Port
    *   WaitCompletionPacket

*   !handle \[*Handle* \[*UMFlags* \[*TypeName*]]]，!handle 0 0 打印各类句柄的统计信息，!handle 0 f 打印各类句柄的详细信息

*   !handle 0 ff Event 打印所有Event类型对象信息

*   !cs 打印关键帧对象

*   !heap 参考 [Memory Leak Detection Using Windbg](https://www.codeproject.com/Articles/31382/Memory-Leak-Detection-Using-Windbg)
    查找heap上的内存泄露(WinDbg版本)
    !heap -s 执行两次，查看两次之间哪一个句柄的增长变化最大。然后
    !heap -stat -h  0x00330000   列出所有该句柄下的内存，找出最感兴趣的那个，执行：
    !heap -flt s 611  比如这里对size为611的感兴趣，列出大小为611的所有内存块，然后想要查看某一个用户指针的创建调用堆栈，就执行：
    !heap -p -a 0143d8c8
    Windows 10上新版本做了调整，!heap -p 命令被移除了，同时如果想要查看调用堆栈，需要使用gflags打开堆页(hpa)，另外，在使用gflags打开堆页之后，发现无法定位到堆内存的泄漏了，基本都是反映0泄漏。
    !heap -l 可以用来检查泄漏的堆块

*   !htrace 打印指定句柄的调用堆栈
    *   !htrace -enable 打开句柄的堆栈跟踪
    *   !htrace 0x49c 打印指定句柄 0x49c 的调用历史堆栈，按照时间上排列最近到最远的顺序打印，第一条是最后一个调用的，最后一条是第一次调用的。
    *   !htrace -disable  关闭句柄的堆栈跟踪

## 反汇编

*   u libmfxhw64!MFXVideoVPP\_GetVPPStat+0x9082f -40 L20 按照地址 `libmfxhw64!MFXVideoVPP_GetVPPStat+0x9082f` 往前翻40byte，往后20byte。反汇编的时候要注意，反汇编代码的结构与源代码的结构可能会有一些变化，比如当代码得到优化时，它会被转换为运行得更快并使用更少的内存。有时，由于删除了死代码、合并了代码或内联了函数，函数被删除了。也可以删除局部变量和参数。许多代码优化删除了不需要或不使用的局部变量；其他优化删除循环中的归纳变量。公共子表达式消除将局部变量合并在一起。可以参考 [Debugging Optimized Code and Inline Functions](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/debugging-optimized-code-and-inline-functions-external)

## 其他

*   ?? expression 计算expression
*   ??  @@c++(sizeof(\_image\_nt\_headers)) 计算结构体大小 (方法之一, for c++)
*   ?? sizeof(\_image\_nt\_headers) 计算结构体大小 (方法之二)
*   dt -v \_image\_nt\_headers  计算结构体大小 (方法之三)
*   ? 帮助
*   .cls 清空屏幕
*   !sym noisy  开启详细输出；!sym quiet关闭
*   .reload /f module-name  强制加载符号
*   !cpuid 本地CPU信息
*   vertarget 显示目标计算机版本信息
*   as, aS 给某一个指令、表达式、内存地址、文件等起一个外号
*   ad *Name* 删除外号
*   .dump /ma file-path 生成dump文件
*   .logopen /t /u file-path 开始记录windbg调试命令和输出
*   .logclose 关闭记录
*   .logappend /u file-name 追加记录

# 内核态

*   ==VMWare + COM 端口 的方式==。这种方式有一个大问题，就是调试的速度十分的慢，拿一个内核调试命令 `!memusage` 举例，执行一次要几十分钟才能出结果。网上有人问这个问题([Faster Kernel debugging for Windows](https://reverseengineering.stackexchange.com/questions/16588/faster-kernel-debugging-for-windows))，推荐一个 [VirtualKd](https://sysprogs.com/legacy/virtualkd/) 工具，不过貌似对VMWare的支持不是很全，本机安装的 VMWare 16 + Win10就无法正常工作。而且从[What is the fastest connection method to debug a windows 10/7 kernel using windbg?](https://stackoverflow.com/questions/62679317/what-is-the-fastest-connection-method-to-debug-a-windows-10-7-kernel-using-windb)里面可以看出，COM端口的方式确实很慢。
*   ==VMWare + Network 的方式==。
*   ==本机调试== 要禁用安全启动选项，`bcdedit /debug on`。不过我试了一下，在本机调试的过程中，使用windbg的 `.crash` 命令无法生成dump。不知道为什么。最后只能是在host机器中打开windbg，通过COM端口的方式执行 `.crash` 命令，成功让系统crash并生成dump了。

> 注意：.dump 并不能生成内核态的 dump 文件

一旦生成了dump，之后执行 `!memusage`, `!address` 命令就很快了。

*   .breakin 从用户态切换到内核态，这个前提是启动内核态

## 参考

*   \[Switching Modes(<https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/switching-modes>)]
*   [How to do hybrid user-mode/kernel-mode debugging?](https://stackoverflow.com/questions/42776503/how-to-do-hybrid-user-mode-kernel-mode-debugging)

## 符号

*   .reload /user 加载user symbol， 仅在kernel mode下使用
*   .crash 生成直接触发系统产生一个crash

## 进程、线程

*   !process 列举当前内核调试中所有进程的信息
*   !process 9382a530 显示某一个进程的信息(9382a530 是该进程的EPROCESS地址)
*   .process 指定当前进程上下文。对于dump，必须是完整转储文件。
*   .process /i EPROCESS地址，例如: `.process` /i 9382a530
*   !thread 0xffffcb088f0a4480 查看某一个线程信息，后面跟线程地址，
*   .thread ffff8e0863d5b080 切换线程上下文到指定的TEB上
*   .context \[PageDirectoryBase] 设置用户模式地址上下文 `[PageDirectoryBase]` 可以通过 `.process/!process` 输出的某个进程的 `DirBase` 取得。

## 内存

*   !memusage
*   !address
*   !vad 打印一个虚拟地址描述符的详细信息，或者打印整个虚拟地址描述符(`VAD Virtual Address Descriptor`)树；`!process` 可以打印 `vad` 地址

## 对象

*   !object 查看某一个系统对象，比如事件，

## 驱动

*   !drvojb `DEVICE_OBJECT` 的信息
*   !devobj 打印 `DEVICE_OBJECT` 详细结构，
*   !devnode 打印设备树中的一个节点

# 远程调试

被调试机器 PC1：

*   用windbg启动程序 `windbg -server tcp:port=5005 exe-file-path`
*   或者attach一个程序 `windbg -server tcp:port=5005 -p process-id`

调试机器 PC2：

*   `windbg -remote tcp:port=5005,server=ip-add-of-PC1`

> 注意上面的 `port=5005,` 逗号之后不能有空格。

如果要设置符号的话，记得需要在被调试机器上设置符号PC1，运行在PC2的WinDbg，如果设置的符号路径位于PC2上，会报告找不到路径，因为这里只会寻找PC1的符号路径；这时候，可以将PC2的符号文件夹设置为共享文件夹，然后最好是在PC1上运行 `\\PC2-IP` 来登录一下 PC2，之后才能在在PC1上访问PC2的共享文件。比如PC2上微软符号的共享文件名是：`\\IP-or-MachineName\Symbols` （这里要记得设置共享文件的权限为**可读可写**），调试进程的符号路径是：`\\IP-or-MachineName\Shared-Folder`，那么可以在PC1的WinDbg上设置符号路径：`srv*\\IP-or-MachineName\Symbols*http://msdl.microsoft.com/download/symbols;\\IP-or-MachineName\Shared-Folder`。

另外，还需要保证PC2的机器设置的网络发现是打开的。参考 [局域网内如何设置文件夹共享给某台特定的计算机？](https://answers.microsoft.com/zh-hans/windows/forum/all/%E5%B1%80%E5%9F%9F%E7%BD%91%E5%86%85%E5%A6%82/3a098515-c722-4123-9acc-b3181959994a)

[Remote Debugging Using WinDbg](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/remote-debugging-using-windbg)

# 脚本

*   [JavaScript Debugger Scripting](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/javascript-debugger-scripting) windbg支持JavaScript脚本。具体的脚本支持可以在这页看到。Git上微软官方的脚本示例代码：[microsoft/WinDbg-Samples](https://github.com/Microsoft/WinDbg-Samples)
*   老式 `windbg` 脚本：[Using Debugger Commands](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/using-debugger-commands)，老式的脚本短小精干，只是过于难写。一般还是首选 `js` 脚本吧。

## 条件断点

在条件断点中，不能使用 `gu` 等修改调试器状态的命令。参考 [Windbg: "gu" command inside of a breakpoint causes warning](https://reverseengineering.stackexchange.com/questions/24696/windbg-gu-command-inside-of-a-breakpoint-causes-warning)。

        bp msvcrt!malloc "bp /1 @$ra \"? @$t1;? @$retreg64;gc\"; r $t1=@rcx;gc"

这个命令值得好好学习一下。 整体上是一个条件断点 `bp msvcrt!malloc "Conditional"`，后面的条件又是一个一次性条件断点：`bp /1 @$ra "Conditional"`，不过这个断点的位置是前一条断点命中之后的函数返回地址；而当前的条件是先计算 `$t1`， 然后是 `@$retreg64`，接着继续执行代码 `gc`。执行完毕之后，仅仅是第一个断点的条件里面的第一句结束，然后是取出 `rcx` 的值，给了 `t1` ，然后继续。

这个问题，官方文档解释是 [Debugging Session and Execution Model](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/debugging-session-and-execution-model) ：

> If a sequence of debugger commands are executed together--for example, "g ; ? @\$ip"--an implicit wait will occur after any command that requires execution in the target if that command is not the last command in the sequence. An implicit wait cannot occur when the debugger engine is in the state "inside a wait"; in this case, the execution of the commands will stop and the current command--the one that attempted to cause the implicit wait--will be interpreted as an indication of how execution in the target should proceed. The rest of the commands will be discarded.

如果是想要执行 `gu` 命令，可以使用 `r @$ra` 来代替。 参考 [Pseudo-Register Syntax](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/pseudo-register-syntax)

举个例子： `bp KERNELBASE!VirtualAlloc "r @$ra; gc"` 代替 `bp KERNELBASE!VirtualAlloc "gu; r $retreg; gc"`。

使用 `.block` 必须在一个控制流程（`.if/.do/.for ...`）里面，不然的话，可以不用 .block。

## 推荐使用 WinDbg 插件 pykd

*   [pykd](https://githomelab.ru/pykd/pykd) 像是官方站点
*   [Python scripting for WinDbg: a quick introduction to PyKd](https://hshrzd.wordpress.com/2022/01/06/python-scripting-for-windbg-a-quick-introduction-to-pykd/) 一个介绍
*   [Heap tracing with WinDbg and Python](https://labs.f-secure.com/archive/heap-tracing-with-windbg-and-python/) 一个实例

## js 脚本

*   .scriptproviders 加载脚本调试器
*   .scriptload 加载脚本文件 `.scriptload C:\WinDbg\Scripts\TestScript.js`
*   .scriptrun 加载js脚本文件，并执行 ` initializeScript`, `invokeScript` 函数
*   .scriptunload 卸载js脚本文件 `.scriptunload C:\WinDbg\Scripts\TestScript.js`
*   .scriptlist 列举加载的脚本列表

### js 脚本中的特殊函数

1.  initializeScript
    当加载和执行 JavaScript 脚本时，在脚本中的变量、函数和其他对象影响调试器的对象模型之前，它会经历一系列步骤。

    *   脚本被加载到内存中并被解析。
    *   执行脚本中的根代码。
    *   如果脚本有一个名为 initializeScript 的方法，则会调用该方法。
    *   initializeScript 的返回值用于确定如何自动修改调试器的对象模型。
    *   脚本中的名称桥接到调试器的命名空间。
        如前所述，initializeScript 将在脚本的根代码执行后立即被调用。它的工作是将注册对象的 JavaScript 数组返回给提供者，指示如何修改调试器的对象模型。
2.  invokeScript 方法是主要的脚本方法，在运行 .scriptload 和 .scriptrun 时调用
3.  uninitializeScript
    uninitializeScript 方法与 initializeScript 的行为相反。当脚本取消链接并准备卸载时调用它。它的工作是撤消脚本在执行期间对对象模型所做的任何更改和/或销毁脚本缓存的任何对象。
    如果脚本既不对对象模型进行命令式操作也不缓存结果，则它不需要具有 uninitializeScript 方法。提供程序会自动撤消由 initializeScript 的返回值指示的对对象模型执行的任何更改。此类更改不需要显式的 uninitializeScript 方法。

下表总结了脚本命令调用的函数：

| Commond            | .scriptload | .scriptrun (Run Script) | .scriptunload (Unload Script) |    |
| ------------------ | ----------- | ----------------------- | ----------------------------- | :- |
| root               | yes         | yes                     |                               |    |
| initializeScript   | yes         | yes                     |                               |    |
| invokeScript       |             | yes                     |                               |    |
| uninitializeScript |             |                         | yes                           |    |

示例代码：

    // Root of Script
    host.diagnostics.debugLog("***>; Code at the very top (root) of the script is always run \n");

    function initializeScript()
    {
        // Add code here that you want to run every time the script is loaded. 
        // We will just send a message to indicate that function was called.
        host.diagnostics.debugLog("***>; initializeScript was called \n");
    }

    function invokeScript()
    {
        // Add code here that you want to run every time the script is executed. 
        // We will just send a message to indicate that function was called.
        host.diagnostics.debugLog("***>; invokeScript was called \n");
    }

    function uninitializeScript()
    {
        // Add code here that you want to run every time the script is unloaded. 
        // We will just send a message to indicate that function was called.
        host.diagnostics.debugLog("***>; uninitialize was called\n");
    }

    function main()
    {
        // main is just another function name in JavaScript
        // main is not called by .scriptload or .scriptrun  
        host.diagnostics.debugLog("***>; main was called \n");
    }

### 重要信息

# 第三方插件

*   [bootleg/ret-sync](https://github.com/bootleg/ret-sync) 一款同步调试器与反汇编程序的插件 （ret-sync is a set of plugins that helps to synchronize a debugging session (==WinDbg/GDB/LLDB/OllyDbg2/x64dbg==) with ==IDA/Ghidra/Binary Ninja== disassemblers.）
*   [blabberer/hwnd](https://github.com/blabberer/hwnd) 不过，看了这个实现代码，可以知道，不能用于调试dump，因为内部实现要去实时获取HWND的信息。

# 工具

*   [PDBCopy](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/pdbcopy) 用来publish公共符号前，删除一些不想公开的符号
*   [SymChk](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/symchk) 对比可执行符号与符号文件是否匹配。
*   \[LiveKD]<https://learn.microsoft.com/en-us/sysinternals/downloads/livekd>) 在[Local Kernel-Mode Debugging](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/performing-local-kernel-debugging)中介绍的一个工具。可以免配置调试内核态进程。

# 资料

*   [Debugging Tools for Windows (WinDbg, KD, CDB, NTSD)](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/)

*   [《Dive into Windbg系列》Dump分析和内核调试](https://www.anquanke.com/post/id/185951)，这个人的其他[文章](https://www.anquanke.com/member.html?memberId=130994)。其中提到的010Editor工具确实是一个神器，使用模板可以查看很多种格式的二进制文件。[宇宙最强 010 Editor 模板开发教程](https://zhuanlan.zhihu.com/p/113936572)，模板[下载地址](https://www.sweetscape.com/010editor/repository/templates/)

*   [mini dump structure](https://formats.kaitai.io/windows_minidump/windows_minidump.svg)

*   [Heap Snapshots–Tracing All Heap Allocations](https://randomascii.wordpress.com/2019/10/27/heap-snapshots-tracing-all-heap-allocations/) 一位Google开发的博客。

*   [Debug values](https://en.wikipedia.org/wiki/Magic_number_\(programming\)#Debug_values) 有一个列表，介绍了0xcdcdcd, 0x1BADB002，...特殊数字的含义。

*   [Win32 Debug CRT Heap Internals](https://www.nobugs.org/developer/win32/debug_crt_heap.html)介绍了msvc的CRT堆的一些信息：
    *   The newly allocated memory (0x**C**D) is **C**lean memory.
    *   The free()d memory (0x**D**D) is **D**ead memory.
    *   The guard bytes (0x**F**D) are like **F**ences around your memory.

*   Toby Opferman在Code Project上的一个系列文章：[Debug Tutorial](https://www.codeproject.com/Articles/Toby-Opferman#Article)

*   [Let’s debug the Core CLR with WinDBG!](https://medium.com/criteo-engineering/lets-debug-the-core-clr-with-windbg-165bc5078706) 对调试.Net的步骤描述不错

*   [Debugging Using WinDbg Preview](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/debugging-using-windbg-preview) WinDbg Preview还是有一些新功能的。

*   一个宝藏博主花熊的几篇优秀博文：
    *   [76.windbg-高效排错的一个小示例(windbg保留的上下文优化后的结果)](https://blog.csdn.net/hgy413/article/details/72867381)
    *   [windbg-怎么让windbg随进程加载自动启动(映像劫持技术)](https://blog.csdn.net/hgy413/article/details/7926981)
    *   [52.windbg-Call Function调试技巧(调用函数)](https://blog.csdn.net/hgy413/article/details/9048447)
    *   [39.windbg-CPU基础（dg）](https://blog.csdn.net/hgy413/article/details/8559701)
    *   [61.windbg-跳过初始断点(调试技巧)](https://blog.csdn.net/hgy413/article/details/12505909)
    *   [36.windbg-!peb(手工分析PEB结构）](https://blog.csdn.net/hgy413/article/details/8490918)
    *   [71.windbg-转换shellcode为汇编](https://blog.csdn.net/hgy413/article/details/50408092)
    *   [2.windbg-PE完整分析](https://blog.csdn.net/hgy413/article/details/7422722)， [15.windbg-dds、dps、dqs、PE文件解析](https://blog.csdn.net/hgy413/article/details/7565428)
    *   [3.windbg-!pte转换地址(ring0)](https://blog.csdn.net/hgy413/article/details/7425750)
    *   [9.ring3-NtMapViewOfSection注入](https://blog.csdn.net/hgy413/category_5784453.html)

*   一个做hack的博主：风格类似 [Red Team Notes](https://www.ired.team/)
    *   [Mapping Injection](https://idiotc4t.com/code-and-dll-process-injection/mapping-injection)

*   [Native Debugger Objects in NatVis](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/native-debugger-objects-in-natvis) 自定义数据结构信息输出

*   [Customizing Debugger Output Using DML](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/customizing-debugger-output-using-dml) 使用 DML 自定义调试器输出

*   [Enabling Postmortem Debugging](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/enabling-postmortem-debugging) 配置事后调试，包括AeDebug。

