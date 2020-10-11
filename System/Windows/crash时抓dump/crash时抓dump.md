[toc]

# 正文

首先要说明一点，即使出现了异常、崩溃，操作系统也是按照一定规则处理的，并不是随便地终止了程序。

我们写的程序崩溃之后，如果没有做过任何处理，那么默认会弹出一个错误提示对话框，名曰Dr. Watson(参考[Wiki](https://en.wikipedia.org/wiki/Dr._Watson_(debugger))，其可执行文件名字在不同代Windows OS中有所不同)，那么具体是什么情况下才会弹出这个对话框呢？所有崩溃都会弹出吗？

/// TODO

[How to disable or enable Dr. Watson for Windows](https://support.microsoft.com/en-us/help/188296/how-to-disable-or-enable-dr-watson-for-windows)

看msdn上，介绍到[RaiseException()](https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-raiseexception)的Remark部分有提到：

*The RaiseException function enables a process to use structured exception handling to handle private, software-generated, application-defined exceptions.*

也就是说这个函数会直接触发一个SEH异常。

另外，这个函数中的Remark部分还提到具体搜索异常的过程：
1. 系统首先通知进程的调试器，如果存在的话
2. 如果未调试进程，或者关联的调试器不处理这个异常，系统会尝试搜索发生异常的线程的栈帧来定位基于栈帧的异常处理函数。系统首先是收缩当前的栈帧，然后向后遍历之前的栈帧。（也就是向调用函数的方向回溯）
3. 如果找不到基于栈帧的处理函数，或者没有基于栈帧的处理函数处理异常，系统将再次尝试通知进程的调试器。
4. 如果未调试进程，或者关联的调试器未处理异常，则系统将根据异常类型做出默认处理。对于大多数异常，默认操作是调用ExitProcess函数。

所谓 **基于栈帧的处理函数** 就是SEH的__try{}__catch{}代码块，参考 [Frame-based Exception Handling](https://docs.microsoft.com/en-us/windows/win32/debug/frame-based-exception-handling)

## 搞定大多数崩溃捕获 - SetUnhandledExceptionFilter

*这部分内容主要参考 [About Exceptions and Exception Handling](http://crashrpt.sourceforge.net/docs/html/exception_handling.html)*

我们写程序一般是通过 [SetUnhandledExceptionFilter](https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-setunhandledexceptionfilter) 配合 [MiniDumpWriteDump](https://docs.microsoft.com/en-us/windows/win32/api/minidumpapiset/nf-minidumpapiset-minidumpwritedump) 来完成崩溃的捕获和dump抓取的。 SetUnhandledExceptionFilter的自定义处理函数的返回值中：

- EXCEPTION_EXECUTE_HANDLER  一般是终止进程，我写代码测试了一下，这样处理的话，系统的Event Viewer中不会记录异常。
- EXCEPTION_CONTINUE_EXECUTION 在异常发生的代码处，继续执行代码，不过有一个修改异常信息的机会。我写的测试代码中，这种处理返回值，最后会在系统的Event Viewer中找到崩溃记录。
- EXCEPTION_CONTINUE_SEARCH 执行一般的异常处理流程，依赖于[SetErrorMode](https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-seterrormode)设置的标记 

Windows上这个处理过程可以抓取到大多数崩溃。参考资料中的[CrashRpt](http://crashrpt.sourceforge.net/)也使用了这个方法。

### 除此之外呢?

但是SetUnhandledExceptionFilter并不能处理Windows上C\++代码的所有崩溃。

我们常见的几种异常类型

1. 访问无效内存，比如空指针，内存访问越界，

```c
int* pI = nullptr;
*p = 1;
```

2. 栈耗尽，比如无限递归

```c
void InfiniteFunc(int a) {
    int b = 2;
    b++;
    InfiniteFunc(a + b);
}
```

3. 缓存溢出，大数据块写入小数据块

```

```

4. 调用C++的纯虚指针
参考 _get_purecall_handler, _set_purecall_handler的示例代码。

5. 内存耗尽，申请内存失败
*目前的操作系统中由于使用了虚拟内存的技术，一般不会碰到*

6. 非法参数传入C++系统函数
参考 _set_invalid_parameter_handler, _set_thread_local_invalid_parameter_handler 里面的示例代码。

```c
// crt_set_invalid_parameter_handler.c
// compile with: /Zi /MTd
#include <stdio.h>
#include <stdlib.h>
#include <crtdbg.h>  // For _CrtSetReportMode

void myInvalidParameterHandler(const wchar_t* expression,
   const wchar_t* function,
   const wchar_t* file,
   unsigned int line,
   uintptr_t pReserved)
{
   wprintf(L"Invalid parameter detected in function %s."
            L" File: %s Line: %d\n", function, file, line);
   wprintf(L"Expression: %s\n", expression);
   abort();
}

int main( )
{
   char* formatString;

   _invalid_parameter_handler oldHandler, newHandler;
   newHandler = myInvalidParameterHandler;
   oldHandler = _set_invalid_parameter_handler(newHandler);

   // When the debug CRT library is used, invalid parameter errors also raise an assertion
   // Disable the message box for assertions.
   _CrtSetReportMode(_CRT_ASSERT, 0);

   // Call printf_s with invalid parameters.
   formatString = NULL;
   printf(formatString);
}
```

里面提到一个设置函数 [_CrtSetReportMode](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/crtsetreportmode?view=vs-2019)

7. CRT检测到异常并请求强制退出进程

Windows中的异常有两种：
1. 一种是C++的异常，一般用try{ /*throw xxx;*/ }catch{}的结构
2. 另外一种异常是SEH，这个是VC\++编译器独有的，不可用于移植的代码。__try{}__catch{}结构的。

不过，SEH的异常可以通过_set_se_translator()转为C++的异常，参考：[_set_se_translator](https://msdn.microsoft.com/en-us/library/5z4bw5h5.aspx)里面的示例代码，将一个除零的异常，转为了自定义的C\++类型异常 SE_Exception 。

对于使用SEH不能保护的代码，就属于unhandled Exception，可以使用SetUnhandledExceptionFilter设置函数来处理，这个函数是SEH的top-level处理过程。不过使用这个函数要注意，如果异常处理函数是在DLL中，并且这个DLL还没有加载，那么行为是未定义的。

**所以，SEH和SetUnhandledExceptionFilter的关系就是，SEH没有包住的异常栈帧，就会被当作未处理异常，交给SetUnhandledExceptionFilter设置的函数处理**

XP中引入的VEH（[Vectored Exception Handling](https://docs.microsoft.com/en-us/windows/win32/debug/vectored-exception-handling)）是对SEH的扩展。如果你想监控所有类型的异常，就像是调试器那样，那么VEH是非常适合的，不过问题是你要决定哪些异常要处理，哪些不要处理。

CRT错误处理过程
除了C\++类型异常和SEH异常之外，还有CRT异常，crt遇到C++类型的异常之后会调用terminate()函数，所以你最好用set_terminate()设置一个错误处理过程。
CRT错误处理过程可以设置：
- [set_terminate](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/set-terminate-crt?view=vs-2019)
- [set_unexpected](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/set-unexpected-crt?view=vs-2019)
- [_set_purecall_handler](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/get-purecall-handler-set-purecall-handler?view=vs-2019)
- [_set_new_handler](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/set-new-handler?view=vs-2019)
- _set_security_error_handler  已经被移除，参考 _set_security_error_handler no longer declared
- [_set_invalid_parameter_handler](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/set-invalid-parameter-handler-set-thread-local-invalid-parameter-handler?view=vs-2019) 比如printf设置了空缓存区，参考示例代码
- [signal](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/signal?view=vs-2019)  信号处理内有乾坤

C\++信号处理 Signal Handling，也就是C\++中的程序中断机制。通过signal()函数处理。
ANSI标准中一共有六种，
1. SIGABRT Abnormal termination
2. SIGFPE Floating-point error，当浮点运算出错时由CRT调用，一般情况下不会生成，而是生成一个NaN或者无限大的数字，可以通过_controlfp_s函数打开这个异常。参考 [Floating-Point Exceptions](https://docs.microsoft.com/en-us/windows/win32/debug/floating-point-exceptions)
3. SIGILL Illegal instruction（*）
4. SIGINT CTRL+C signal 
5. SIGSEGV Illegal storage access （*）
6. SIGTERM Termination request （*）

标*的MSDN中提示说Windows NT不会生成，只是为了兼容ANSI；但是如果在主线程中设置了SIGSEGV信号函数，那么就会由CRT而不是SEH设置的SetUnhandledExceptionFilter()过滤函数来调用，并且有一个全局的变量_pxcptinfoptrs包含异常信息。如果是在其他线程的话，异常处理过程是由SEH的SetUnhandledExceptionFilter()过滤函数调用的。

除了函数之外，还有编译链接选项上的一些事情。CRT可以以MD（动态链接）和MT（静态链接）的方式编译进模块（exe/DLL）里面。参考：/MT、/MD编译选项，以及可能引起在不同堆中申请、释放内存的问题，/MD, /MT, /LD (Use Run-Time Library)。
MD的方式时推荐的，多个模块公用一个CRT的DLL库的方式；以MT的方式使用CRT的话，需要把函数写成static，并且使用/NODEFAULTLIB链接标记，链接到所有模块中，还需要每个模块中都注册CRT错误处理过程。

## 为什么调试器可以抓到所有崩溃?


## CRT, C++ STL, 系统API的两两关系



## 进程内捕获异常 VS 进程外捕获异常

参考资料中提到的[google-breakpad](https://chromium.googlesource.com/breakpad/breakpad)介绍到，进程外异常才是安全的。

# 理一下思路

Windows上的崩溃，

# 测试代码

Windows上抓取minidump的代码可以参考 [这里](http://www.debuginfo.com/examples/effmdmpexamples.html)。

下面是为了验证上面介绍过的各种情况写的代码。

```c
/// in main function, setup unhandled exception filter for a minidump.
SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)MyUnhandledExceptionFilter);

LONG MyUnhandledExceptionFilter(_EXCEPTION_POINTERS *ExceptionInfo)
{
    HANDLE hFile = CreateFile(_T("E:/MiniDump.dmp"), GENERIC_READ | GENERIC_WRITE,
        0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
    {
        // Create the minidump 

        MINIDUMP_EXCEPTION_INFORMATION mdei;

        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = ExceptionInfo;
        mdei.ClientPointers = FALSE;

        //MINIDUMP_CALLBACK_INFORMATION mci;

        //mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MyMiniDumpCallback;
        //mci.CallbackParam = 0;

        MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory);

        BOOL rv = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
            hFile, mdt, (ExceptionInfo != 0) ? &mdei : 0, 0, NULL);

        if (!rv)
            _tprintf(_T("MiniDumpWriteDump failed. Error: %u \n"), GetLastError());
        else
            _tprintf(_T("Minidump created.\n"));

        // Close the file 

        CloseHandle(hFile);

    }
    else
    {
        _tprintf(_T("CreateFile failed. Error: %u \n"), GetLastError());
    }
    //MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, );
    return EXCEPTION_CONTINUE_SEARCH;
}

class CDerived;
class CBase
{
public:
    CBase(CDerived *derived) : m_pDerived(derived) {};
    ~CBase();
    virtual void function(void) = 0;

    CDerived * m_pDerived;
};

class CDerived : public CBase
{
public:
    CDerived() : CBase(this) {};   // C4355
    virtual void function(void) {};
};

CBase::~CBase()
{
    m_pDerived->function();
}

void myPurecallHandler(void)
{
    printf("In _purecall_handler.");
    exit(0);
}

int coalesced = 0;

int CoalesceHeap()
{
    coalesced = 1;  // Flag RecurseAlloc to stop
    // do some work to free memory
    return 0;
}
// Define a function to be called if new fails to allocate memory.
int MyNewHandler(size_t size)
{
    printf("Allocation failed. Coalescing heap.\n");

    // Call a function to recover some heap space.
    return CoalesceHeap();
}

const auto kBigNum = 0x1fffffff;
int RecurseAlloc() {
    /// make new failed for the first time.
    /// show 3 versions of new operation:

    /// version 1: process crashed.
    int *pi = new int[kBigNum];

    /// version 2: process not crashed.
    //try
    //{
    //    int *pi = new int[kBigNum];
    //}
    //catch (std::bad_alloc) {
    //    //
    //    return 0;
    //}

    /// version 3: same as version 2.
    /// modify as std::nothrow version new, MyNewHandler called
    //int *pi = new (std::nothrow) int[kBigNum];

    if (!coalesced)
        RecurseAlloc();
    return 0;
}

/// function to make kinds of error.
void FooCode() {

    /// Disabling the program crash dialog 
    /// ref: https://devblogs.microsoft.com/oldnewthing/20040727-00/?p=38323
    DWORD dwMode = SetErrorMode(SEM_NOGPFAULTERRORBOX);
    SetErrorMode(dwMode | SEM_NOGPFAULTERRORBOX);

    /// access violation -- dump catched
    //int* p = nullptr;
    //*p = 0;

    /// infinite loop -- dump catched
    //int a[2048] = {25};
    //a[2000] = 50;
    //FooCode();

    /// throw a C++ exception -- dump catched.
    //throw std::exception("i am an exception");

    ///======== crt error handler ==========
    //-- invalid parameter -- dump not catched
    // Disable the message box for assertions(in debug mode, not in release mode).
    // If not diabled, it will popup dialog : Microsoft Visual C++ Runtime Library -- Debug Assertion Failed!
    //_CrtSetReportMode(_CRT_ASSERT, 0);
    //const char* formatString = NULL;
    //printf(formatString);
    

    //-- pure call -- dump not catched.
    // popup Microsoft Visual C++ Runtime Library, just only for debug mode,
    // and does not popup any dialog for release mode.
    //_set_purecall_handler(myPurecallHandler); // used for catch the pure call exception.
    //CDerived myDerived;

    //-- new -- dump catched, and popup nothing no matter for debug/release mode
    // Set the failure handler for new to be MyNewHandler.
    //_set_new_handler(MyNewHandler);
    RecurseAlloc();

    /// and more...
}
```


![invalid-parame-crash-dialog][invalid-parame-crash-dialog]

![pure-call][pure-call]

# 参考资料

一个被广泛使用的开源项目 [CrashRpt](http://crashrpt.sourceforge.net/)，文档中有异常和异常处理的一篇好文章：[About Exceptions and Exception Handling](http://crashrpt.sourceforge.net/docs/html/exception_handling.html), 国内2011年就有人[翻译](https://www.cnblogs.com/hdtianfu/archive/2011/12/27/2303113.html)了的。

谷歌开源项目[google-breakpad](https://chromium.googlesource.com/breakpad/breakpad)，docs中先看[getting_started_with_breakpad.md](https://chromium.googlesource.com/breakpad/breakpad/+/master/docs/getting_started_with_breakpad.md)这个概述文件。

张银奎老师[软件调试](https://book.douban.com/subject/3088353/)中第11章中断和异常管理，第12章 未处理异常和JIT调试。

[深入解析Windows操作系统：第6版（上册）](https://book.douban.com/subject/25844377/) 第三章系统机制中 3.1 陷阱分发。

[Matt Pietrek](https://en.wikipedia.org/wiki/Matt_Pietrek) 在 Microsoft Systems Journal中的一篇文章 [A Crash Course on the Depths of Win32™ Structured Exception Handling](https://web.archive.org/web/20040603050159/http://www.microsoft.com/msj/0197/exception/exception.aspx)

[Under The Hood - Matt Pietrek](https://docs.microsoft.com/en-us/archive/blogs/matt_pietrek/)

MSDN- [Structured Exception Handling](https://docs.microsoft.com/en-us/windows/win32/debug/structured-exception-handling), [Exception Handling Routines](https://docs.microsoft.com/en-us/cpp/c-runtime-library/exception-handling-routines?view=vs-2019)

[SetUnhandledExceptionFilter拦不住的崩溃](https://blog.csdn.net/limiteee/article/details/8472179) 介绍了一种不被CRT替换掉自定义异常过滤函数的方法。

[Why SetUnhandledExceptionFilter cannot capture some exception but AddVectoredExceptionHandler can do](https://stackoverflow.com/questions/19656946/why-setunhandledexceptionfilter-cannot-capture-some-exception-but-addvectoredexc) 介绍了VEH比SEH高级的地方。

[Getting an exception call stack from the catch block (C++) ](https://docs.microsoft.com/en-us/archive/blogs/slavao/getting-an-exception-call-stack-from-the-catch-block-c) MSDN上的一篇blog，介绍了C++的try-catch抓到一个异常之后如何获取到call stack。有点高深，没看懂。

[windows下捕获dump](https://www.cnblogs.com/cswuyg/p/3207576.html)  有一个写好的例子。

[Integrating Crash Reporting into Your Application - A Beginners Tutorial](https://www.codeproject.com/Articles/308634/Integrating-Crash-Reporting-into-Your-Application)，
[Add Crash Reporting to Your Applications with theCrashRpt Library](https://www.codeproject.com/Articles/3497/Add-Crash-Reporting-to-Your-Applications-with-the)

[What Every Computer Programmer Should Know About Windows API, CRT, and the Standard C++ Library](https://www.codeproject.com/Articles/22642/What-Every-Computer-Programmer-Should-Know-About-W)

[Crt, API, STL, MFC, ATL之间的关系](https://blog.csdn.net/acelit/article/details/58584706)



Raymond的一篇文章 [Disabling the program crash dialog](https://devblogs.microsoft.com/oldnewthing/20040727-00/?p=38323)






[invalid-parame-crash-dialog]:data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAagAAAEbCAYAAACP7BAbAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAADk1SURBVHhe7Z15uB1Fmf/z3+jcGRllnNXxGdQZ74AKKglrWGQJYb2oCIMomVH8OeM88jjPyMgyQFSIElkDSAAhCUsSLgECIjH7QkhIwk1CQnZCEgJkIWERV+aZ91ff6q7u6urq5fQ9556+N9/P87z3dtfy1lvV3fU91afPOYP+7u/+Tlz70Ic+pO3II4+k0eTmKw/a5+2iCz7TkJ144onesSyyM844Qx566CEabZ+x3bt3y969e+Wtt96Sd955R37zm9/I7373O/n9738vhQJFCCGEtAII1GuvvSa7du2SN954Q95++2359a9/Lb/97W/9AmXEiQJFCCGklUCgtm3bpkUKKymI1K9+9SstUFhFZQrU3/7t31KgCCGEtAwI1Isvvigvv/yy7NixQ/bs2aNv9ZlVVEKg7NUTBYoQQkgrgUCtX79eXnrpJXnllVf0KurNN9/U70VRoAghhLQNCNQLL7ygV1Hbt2+P3osyt/m8AgVxokARQghpJRCo559/XjZs2KDfi9q5c6d+oo8CRQghpK1AoFasWCHr1q2TLVu26Icl8D4UnubD4+aRQBlxokARQgjpCyBQPT09snbt2kigXn/9dQoUIYSQ9mIEas2aNfpBiVdffZUCRQghpHHwlN27774b7qVBHsqUBQL13HPPpQQKj5rnCtTf/M3flBSozTJz7FgZO3NzuJ9kb0+3jB3bLT179Y50m+12sXmmikfF290jeWFsnjlWut1AURf9bEk/MI55PvdKT7eKG7GXiL8a4bG0LOOwVqel50DGGCbaLBpnQkgWRoB8IpWXl4URKDzJ10KB6pbubt9Fbya8ukwIQTzxpJszWRkxsoBoNX3CjsiJRU+wTtsqrafpsTgxaDGfqVJ7Q06/mk6ZtvoyHkIGHj4hqiJOoM8EauZMJVLuVa8n+Zk1mhDcySlnstKiYE/OKNvbyTqPrFiClVPrhNGmgfEpTTN8lKVMW30ZDyEDE1uQqooT6DOB6tmL//YEjonVpJsJwZ0ckretgkk4KAPBi169hyuI9G0nX30k+8rDb5zW3dOT2E/fonSEIbGisvuR3Ye4n8l9rMTS7bp1QlJCmQXiyCtXlA+cGNDn6FZiXp/C7R6suIJ+BS9WkO721VNvc3y8UCS4LWz7CUkcV19fbN82+W3GbQR59rmXd6yCcrh7YJ0nIBw3QgYyRpiqihPoQ4EKLuboQsVkoic3z+Sgt4OJPbXq0mXcScPxa3wkRMOQUz7RPnD3k2CiNHEkb+9Z9TJjKNOOnZ5RJhrHIpolUL4JGeT1KaxnymNMIgEpUc/0T9ezjn2eH5XnP3fstgw5babOD985CdI+EnFaY9XaW8GE1IN+J1D2ZBpfpO6FbZUt8yrYUy7yrfOcCSWvvOs7te8Q9QeTe0Y9XwxF7YQTcWAmPSMWT38ShO3H/gKLJsii/ARuv+x28/rUijxg7fv6keqEW9+Q16ZKyTw/FKWOFfbNWKntUi8oCOm/GHHCf3u7UfpWoMxEjtsn0UVq51vbmROvXV7hKee+QjW3hHRabnnHd2rfxdcfkK6XiCGvnUR8tvBlxYIyyf76QTnfeBqK8kEyBnsFmdunluQBaz/zfLFx6xvy2rTH18lr4FiZsUqOGSEDD58gVRWpPhYo7Dq3aRL59nYwMaQvZvfix741QWdMVPHEkFfe59veT2OEJxmnv14cgz3pKfSYhOWxbcROx2b85MQSjmlCpFTd5FN8RQJUlA/yxienT7n1quYBt6zvfLFx6xty/OSdH40cK+Sr8Z2ZWGkTMrDIE6IqItX3ApWaCN3JwS6L/fiWTTD5ZVz8UTkrL5y4A7PazCrv8Y3VlS6XmP0tEhOTwfKTFYOdnniSMZjodXpiQvP02ybRJ9RtxW2kdAxaoE1bmX1y6yX3k2Ns5+XXS+27Y5A6Zihv5SuLX7Qk2wwecAjKxG7c9hs7VrqfWecRIQOAIgEyIlWWPhAoQghwbz0TQvKhQBHSF2B115JVLSEDFwoUIS3F3AZM3/IjhORDgSKEEFJLKFCEEEJqCQWKEEJILaFAEUIIqSUUKEIIIbWEAkUIIaSWUKAIIYTUEgoUIYSQWkKBIoQQUksoUIQQQmoJBYoQQkgtoUARQgipJRQoQgghtYQCRQghpJZQoAghhNQSChQhhJBaQoEihBBSSyhQhBBCagkFihBCSC2hQBFCCKklFChCCCG1hAJFCCGkllCgCCGE1BIKFCGEkFpCgSKEEFJLKFCEEEJqCQWKEEJILaFAEUIIqSUUKEIIIbWEAkUIIaSWUKAIIYTUkn4tUDt37pRFzzwjkydNkhtvuEGuuPxyufjb39aGbaQhD2VQlhBCSP+hXwrU+vXr5aHJk+Xqq66SCRMmyNy5c2XDhg2yd88eeffdd7VhG2nIQxmURR3UJYQQUn/6lUDt3r1bHnvsMbnxxhtlzpw58rra371+nWyfNUs2TZwka2+7XVZf+yPp+Z+r5PnRP5H1E8bL1tlzZZcSpdd37dJ1UBc+4IsQQkh96TcCtW7tWrntttvkl9OmyRsqwFfmL5Ctj0yRHT+7R7ZcfLFsveAC2fb5L8rWs7vkpbPOko3K1nSdLcvPPlsWX3ihrBo/XrYsXiSvbd+ufcAXfFZnpHQOGiSDbOscGeblgXod0jUj3G0VM7qko+XtuH3Bfqf62yzSY9yR2yG7/SrjbNdpdl8IIY3SLwRq5cqVcvNNN8mqVavk9RUr5KXJD8nrEybIyxddJK8qAdquxGj7GWfJljPOkJeUbVK2cfjpsu7UU2XN8GGyetgwWTp8uCw47TRZccMNsmPZc9oXfMJ3NdwJcIZ0dZQRqSoTZ+PM6OqQzs7Oggm9t7S6L73xX6WuXQfHkwJFSDupvUBhlQMh2bp1q2x78knZ/uBEeeW/vyevhMK07cwztTBtPv102aQEaKMSorWnnCIvKFFaddJJslLZ8hNOkGWf+5wsPfZYma+2f3HSybJpyhTtE76rraR8E6Cbhv3w1X8kXGGZrs5oVRBrmq++319HV5eTZ2MmV1W+o0vtxUC4gnbjydeXlhd7Z6cq39GhVmhhvrJACLPjHeTrlzUGfiF1/RnCFwOpunb5nFgSLyLs9E6rDgWKkHZTa4HC+0S4FYfVjhane+6Rbd/4f/JqlxKlk0+WDUOHyqYTTwyESYnS+mFKmFT6qhNOlJVKiJYrUXruhONlsYpz4cGflgWDB8uCo4+W2arek8ceJ5seflj7RhuNvyflmzyDidPMfyM7zTbS7YnTmiRHYpI2E6FvUo334c9MxoGouO2H4PZe6D+OAcCfO+n60ux66dj9ggCy49X9jMQy8OMfA5uwnGVxXwwo4xu/dCzp/gTpiRijOhQoQtpNrQUKDzPg/SLc1tt4443y8je/qVdMG487RnZedJH89vHHZbv6//xRR8haJUyrlSgZYVp63HGyRK2Ynv7sZ2WpqrNNidvyL18g0w/8J5k5ZIhMO/wImXrMsbJz6VLdBtpqjOQEGGALVNbk6tbLnlTL5yVJiBIm3WgniC+5gvClwXejsQN7v2qeTVa6QouJic/n191utD8UKELaTW0FCo+D44k7PBCx5vbb5eXLL5ctp50hG48/Xl698EKR114LCu7cKZvOP1+WKCEywvSsWiEtPmaozD/kEHlWraxenDtXNu99XbYooVv0xXPkic5O+flnPiNTDz1UnvrCF3UbaKuxR9DdyU2hH0zwvZq3yZsUq+bZID05Gburk2D1lZeW5zsvBnu/ap5NRnpqNebzm7Vt46ZnlSOEtIPaChQ+szR79mx5dcmzsvHSy+Sl00/X4rTl3HNFdu0KS4W8tkPWfvnL8synPiXPHnOMLFRxzfvUJ2Tx8FPlxfnzZcPLL8tq1cFVGzbI+qVLZMawYTLlH/5BHj7oE/LIZz4r6+5/QD+CjjbL45vc7Ntfwcok3jcky2lhiCZbewWmSN1yyrodZZFYMQUkVlQa+HLr2ml5sbt9zt7Pv8WX58eQkW73MTEOdnl7O6s/eWOK+lxBEdJOailQ+NYHfLB2t/q/YeJE2ThihGwcdpKsOfpo2XzueSJvvx2WjPndjh2yWonULLU6mnXQgbJw+HDZuGCBbNi+XXduRU+PvKLKvPPOO/KoynvwgAPkwQMPlMlKpKade47+nBTaLP+NE5jAkisVRxfU/IcVlbuKCSZO/aCBTncmYD1JhnUSb9orbH9uXkhajBTRhG7FHBXypSlyYrfbRHsoE0zybr7lO5Wet2/ISw/9JsbBLu/U9fZHkTmmqE+BIqSd1FKg8NVE+PaHXes3yPJvfUvWnniirDnheFmFhx4OPVQ2fv3rIm+8ocv+7//+rw70HfX/za3bZFlXl8w77jgtTuu2bpUXVq+W5cuXB+KkhG2Cyhv7J38iE/bbT8Yp+5myyQcfIjtWrpT77rtPt90fSK68CCFk4FFLgcL35+ErirZOnyXLz/mSrD7+eHn+2GOlR9myoUNlwYGfkFVf/ar8Zvdu+YMqj2Dx1UY733xTXn3+eVkza5a+nQdhWvLss7LtlVfkV6rMOBXvHUqc7uvokHuV3RPa/apPq+6+W+bNm6fbrivB+0RmFeBbWRBCyMChlgKFL3nF9+i9cNfd8tzw4bJi6DHSM/RoWXL00fqR8UVHHKVv5S274ALZo8RnzzvvyKvq/+bNm2X9pk2yav163anFixfLViNOStywcrr/j/9YxitRGhcahGrC/vvLXLVSQ5tomxBCSPuppUDhm8ixIlp69VWy7PjjImFafOTh8vThh8v8IUNk7hFHyFMfPUDmn3qqbF66VDZt2SJr1qyRVWoF1aM69KxaOW3dtk3eVh2573Ofi8TJGFZRMIjV+Pe9Tx474UTdJtomhBDSfmopUPi5DHwj+ex//mdZctRRWpyePuwwmT94sMxV4jQbn2P66Edl6nvfKxOV2MxWq6zVSpxWrloVrZwWPP207FGC88SFF+rbeij3oGUQKfP/fiVSD37yk7pNtE0IIaT91Fagfv/738u8b3wjWDFBmA49VGar/zMPOUR+vv/+MkWJ0yQlLk98+MOyAp1Q4rRs2bJAnBYskHnz58scJVJLH39cfn7QQVqIUB422TUI1Kc/LX/4wx8oUIQQUhNqe4tv967dsuiK/5E5SqBmQ5yGDJZpBx4ojylhejgUp6kqlhVKgJatXStLly6VxYsWyQII1DPPyLw5c2TWrFkyc+FC6Zk2TZ78p3/SdVAX4vaoZY+pFdajw4bJHtVx3uIjhJB6UNuHJNauWy8rx46VGUqg8NVET33iE1pYsOLBauhRFcfyqVNl6Zo1slStnBYpcdq6fbvsVcE/9fDDMhMrqNmz9dOAzyxfLi/MmyfTPv5xLVCPh/Zz2HveI0++730yR62c9LdX8CEJQgipBbUUKDzqjW+R2DRjujx1xJEy7eCDI3HC+0YPf/CDsnzKFC1OS9TK6Rm1YoI4vbF3r4xXgjbhIx+Rad0Py1y1msKqCh1cv3WrbF2yWGZ+7GNalH6pbLqyGcq637efvDBhvBazOj9mTggh+xK1FCh8WHbc+HGyfe0aeUK1MeUDH9C357Byemj//WV5d3cgTkuWaHHaEoqTeZQcItbd2SlzHn9cetatk3Wq7JYtW2TPr38je1aulLkf+rDM/qM/kjnK5ip76O8PkJ0vrJL77pvQbz6oSwghA51aCpT5qqNXXn5ZHj/5ZJn053+uxQmfW5p8wAGyZNYsWbxihSxcuFC2qjJv7Nkj9xxzjH5aD+JkHoR47KCD5Dm1Ktq2a5f+OY3f/d//6UfJ8Rmqp5UwLXrve+Tp/faTpT8apX8+fuTVVzfwVUeEEEJaSS0FCuCLW/EFri/cdZc8qNocr8QHH6q9U9nEwYNl9pNPyksqWIjT3UcfLT9V+RAxCBTECbcE8UDFk//4j7K5p0feVT7fUqus6aecIjPf8x55TpXpUb663/9+2TJ3tsyZ2+iXxRJCCGkltRUo++c2pp50ktz/F38hdylBwS2825VN+OxnZcUvfyl3HXmE3Kb28YFbCBQ+79StDOKE95qeUjb/4INl2yOPyJzTTtO39laqsqtUnWX7/7lMVYJV7ec2CCGEtJLaChQwP1i4Y8kSue9j/yD37r+/XilBkGA3h//xfXq4/QeBwu09rJ7wlN40JU5YLeF9JvxfqP5DmNb+6Z/Kyg98QCb+/d/LbtX56UroGv/BQkIIIa2k1gJl/+T7+kmT5H4lKHcpkbolFCaIFVZUtkDp957C1ROe0IM4PaPSlqk0iNM6JU6rlI9JKv7NUx6R1atXV/zJd0IIIa2k1gIF1q1dKzffdJNs3bpVNiiRGvfRj8rdH/yg3KKEBiJlC5R+BN1ZPS1Q9qwqs0LZGlVnuRKnyR/5iGx+9FHtE77RBiGEkHpRe4ECK1eu1EKClRRu9+GLXcf+5V/K3e9/v35yD+9NuQKFzzjhMfLF7+2QHiVMy1TZR//sz+Tx4afK7uXLtS/4hG9CCCH1o18IFMAqB7fi8J4UHmp4ae5cefq/L5VxB3xE7vrAB+R+JT6TlFg9rETq58p+qWy2Eqap++0nDypxmvalL8mmKY/ouvABX1w5EUJIfek3AgXwPhEeZsATd/iePfwk/M716+XFJ5+U50aPlqfO/2eZdMRhcv8nDpKpaqU07zvfkbUTxssOtVravWuX/uoj1IWP3r/nZP3seL/9AUG3D+7Pxav81K/2tqLf8Gn7wX6jP7fui7UdNKMvRbhtNIJd1zdmyLePr7LkSZFBb2Ly4YutHbj9wn5dj+fAo18JlAGPg+MzS/gwL34aHl9RhB8bxIdw8ZMZMGwjDXkog7Ko07xHyZ0TY2SnupibfeK2muKTG7/imxKtpve7ORdZOtZ20BcTRm/aSNYtPL5KIro6yohU8/vN41mGvoivffRLgTLgWx/w1UT4/jx8ySu+iRw/lwHDNtKQhzLN/4YI98TAPibqIL2zs0MGRa8AkWZekbonU5zX0dVl+XT9hBNFVNY4scqF6SM7w3Lm6oaIeK/0oG4yHoVdfkaXdCTqunWw7+t3VrwAZU2MnfF2VM7XhilvYvGkpWIF6XIYHxOP/hn9PJ+ZaSq+LohzkBdkWWWVFfYllZ70mRwzg68NJz2KM+yfTjfHyKlbeHyBm+ZrKyyTGhPgq+/3l7gGeDyD9CjOEsczRdpPY+PVXvq1QLUXHMz4hNQHWk/MwUG2Txb7hNCTv3XrInWyRD7TfmKQZ1YtVjlc0Gpbn1t6OyyTK1DhCanNX36kEp04DNQp1+8YO174cy+ApE93P10+SAuSIIR2WdtPVjkTT1FcefWtshivyE9xX/znQp5PF7eNwG92P218dbPjDQhebMShtW5MktcA8pKx5PfT/A9A2X3xeNo0Ol7psu2FAlUZHFx1AkZW7oRO7jeSp9AnuWnPV66gfooyZRSq3Vh4UKdMvxWF8RrcNHs/q7wdg7mwkJUXa1wumAhtQW2kHbesvd+KPBdfWV+cmGjUfjQ4wOM3NWZuu4GfuO++tnwxmf2qedjl8YzjLHk8I7L8ZI2Xv2w7oUBVJuvEcNPz9hvIw2SfeHXmK5dT30uZMkBdGJ2+tm2c9FLxGty0RsvblIlVgfgSF2Ej7bjp9n4L8sKVsZ44dMB5ftIEk1HOC4miMbNX45lt5cVUNQ/weLoUH09DTl6p8Wo/FKjKZB3QdHr2bYDgFVEiL6rr+EGeOZsyy2Vs23UTpGPVeMrjogiSMuq46ZnxOn3WuD7tfV95X1pMHGtWOfjHBW7+g0baQb04XU8YXjEGyf38W0LZ9ZK4efnjEeSb8n6/2ccX+7bvKmMS1IlOqbzzIZEXwOPpgnxT3tdO3hgAU6ZM2fZCgaqMe2IYfOlIC18xuXmJV1N4YCDvxCsql7GNCyeaHWwsn6HpE9RXHnHqNLsNGzfd8p2IF9h5QTu40KP2c32FcdnjFl1kIVGs4bZTzp5U9GQU9bVsO0F85sEU95iW7ksqPW8/SbINRWacYVrUR09dEI2ZHV9gVtWACmOiGg3Tlbnng+0vda4ootjCbVOWx1OTjs+UVzQyXt422wsE6oEHHpDjjjtO5s+fL+vWrZPzzjtPpk6dKhdccAEFqi9JvnKrH+4b1nWmtbHmTzb9lToc36xrgMdz3yRLoE455RRZs2YNBarV6AsyetXCi6R/wAmtmbT/GuDxrCtZAnXdddfxFh8hhJD2kSVQkyZNokARQghpHxQoQgghtYQCVRnctzb3zQNLP0XT6nvbdlt5tOMeu2lT/a/04EdRzFX95mG32Sz/rYizClX6Ztfx4fgsPBfLtluGotjK0iw/pBVAoG644QYKVOOUObHrcvK3I464TbwJbj0VW5LimKv5zSPZZrP8Nz/OKlTpW9ExKD5GLs0bi8bb9tMsP6QVjB07VoYOHUqBapysE9tOd8tgP1xxmas09zNK6mIOP5uB1Zn5vENc3tOW98sp3XLuJ8j9fuMvfs3zDbJ96GL4fEWqj24du80O6/MYzmc2dFr4Sj3l12pT49tvoE1P3KkYEj7M6sBpJ+Un+FBkoi1NGG/eF65684DTZiKuKn2z/Jk6Grudkp/bs9OLzoWUD9++Xd7EFubljV0pPy5WGWtcqx0j0ijnn3++vsV31llnUaAaI3lyxydjeJLqc93eVntKYIIy1ifBCz5EG00mxr/eTk6OcVsqDuNLfzDSLWd/Aj3AG1PoKzUpeX3n+Yjb8n3pZ7LfvjbtOtiP2zQk/frq2DE02ibqFMXg+kAdtx3Xj43tM/CVfQzzj0Fr+ubWsXzqGEx+smyqnHVrz3cu+Mu67dv7vry8sSvrJ0nWuFY5RqRxcIuP3yRRiawTO+vkD09eyxLnfYo8P0XbIJ3XoV61J9tEui+mPF/A9V3Ch7pY8ybLdJq7H6463EHL9WvvV2lTkfDvi6GED5Dwo9CTlxkzUz7PVyN5IK98SK/7Zu9nbQNnv/IxK5sHqvqx8eU14ivPNykDBaoyWSdf1gnb6Mlaxk+efzdPvUrFrZ64gMKtY8jzBVzfZXyoCTD3Sz/z2ogJbkPZr0rz/Nr7Vdu0/QckYyjjA1h+IE7eVUKer0byQF55Q2/7Zu9nbQN3v+oxK5sHqvqx8eU14ivPNykDBaoyWSdf1gmrLkr1CjUpEApMVu6qQJPlp8w28OclbqdkxZTrC9j7ZX0Ek1/QVV+dojYNqJvvNxpOvUrJi7Ncm7F/gx2DW8fXTkDkxz7miRjhK66rxcJ5X8uf16a+pWKPy+bd4gN2u9llg/ai+HLaC/bzx6e8HyPQWeOa1U5eHqkCBaoy7oltsNOdMuF7ScFtnfAiwMWSnCFCsvyU2QZZecFFF7XpiynXF3D2S/lQoFw8S6j8sI5OS5fX9/9VfnDBu+UtbL968jHlir6ktqjNkMi/L4a0D385hddP+kED/WCDzndjz8oDbptBebtM433z1EH5qFyFhyQM9jHLK5tzPNPnR874NOTHnMPAis0a12rHiDQKBYr0KdkPC/SOVvk1NMt/vp9ggvPn5+X1jlaPXRbNbbd145OkPcdoX4UCRUht4ORXHQrUQIQCRQghpJZQoAghhNQSChQhhJBaQoGqjO9+M9LsJ4CaBfyGTxIps5/CMk8gBdaKtgkhpD1QoCrTpjdE9WO+RojwyDjflCWEDEwoUJXJWkGZtHDb+uLIxOdPdH648jGfB8n8TJQFBCrxAUQKFCFkYEKBqkwZgXLEx7oFh1tzQZYlMjkCpT+VDn+JT6aHH7oNhS4pgIQQ0r+hQFWmjEDZ+R7xsqxo4RSRuMVn44uHEEL6LxSoyvgEwRWhPIGqLibx6itJVjohhPRHKFCV6Y1ABbfmUrfkyr4HxRUUIWQfgAJVGQiCdZtOC0tZgVJooTH1Q8HJEij9/lXcVlQk4YOrJ0LIwIICRQghpJZQoAghhNQSChQhhJBaQoEihBBSSyhQhBBCagkFihBCSC154IEHZOHChVqk1qxZIy+++KK8/PLLsmPHDi1UFChCCCFtgQJFCCGkllCgCCGE1BIKFCGEkFpCgSKEEFJLKFCEEEJqCQWKEEJILaFAEUIIqSUUKEIIIbWEAkUIIaSWUKAIIYTUEgoUIYSQWkKBqozzk++1+cl1xBX+hHwLGNkZ9LUj/i37PsD9+Xyb1vY3jR1LXlw2ZcsRQmwoUJXZFyeddvU5r90Z0tVBgSJkIEKBqox/0sEKw6wuZnR1yCC9rArLdnV6VltBXmenKtvRpaZbkxauzKxlmfan0+MJOZ3mxmX5SqUnY4pWRSNVWmo5aPtx48/2H/fL2ldl0ZZZjcVtQWyMLxNPst30yq2qQAXxePtv8py+BPsF26ljjHQ3/qBs8pgTQlwoUJVJTjxJgcC2+W/SVBkzEUMAnDx74sXEHRTF5GtPgO5EnJVm6gS+It9o1xVBX0xegQJJ36DIf3LSD/dndEmH2tZN6G23DwDl7TFKthtjC1RS4Ix5u6J9ZvQ/1Z69n7fdiL/kMSeEpKFAVcaddGLMqiY5ObsTlG+SA8HklZ5gw8k3Mdv60op8l8nLoqhOWf855fTEbvqe5cvGFqhGKBsfyMqrUge4+4QQHxSoyuRMMuEkG+tG8yavQPySE3IyrazvxtoNKKpT1n/GdmoFluUri0ZXULbPvPay8qrUAe4+IcQHBaoyWZMM0iEW5r9Ji1dUWlC8EzEIJtns2z/Id9u105L+8m/B2X6sfZTzzupuHRSt4D9r224X297yLshr9goqOAbREGTG4m6XPcbuPiHEBwWqMsGEZL9Sx+RkT9h6ktKzXDAhmQcEvA8r2JNV+B5NUDYpcjotnjkz0lz/YZncdq39BgSqkv/c7dBXZ2eiPsYV6ZEYRqBOswUKuxAlXyxuvMlt/zGGOzt+t21CiA8KVJ/ACWngw2NMSLOhQPUJnLwGPjzGhDQbChQhhJBaQoEihBBSSyhQhBBCagkFihBCSC2hQBFCCKklFChCCCG1hAJFCCGkllCgCCGE1BIKFCGEkFpCgSKEEFJLKFCEEEJqCQWKEEJILaFAEUIIqSXfveS78r3LvidXXnWl/OIXv9ACtXTpUrnl1lvk5ltupkARQghpD74VFATqzrvvlC1btlCgCCGEtAcKFCGEkFpyxx13UKCqgR+oC38SPLT0z5E3AvyV++ny7J8/byXuD/KVj7fv4I8GEtI/rtVymPegxo0flxAovgdVSLsmw32t3UboDzES0moGznWAW3yPPfaYjPzBSLnzrju5gipP3kmAPLOysssEdTo7O2RQR5ckq9r+wu2uTmd1ZvsdJJ3RS6Ky7Vn7qix8mtXYoMjZDOnqML787cZpblumTDqGdF989NZHUTm/f4xBXC70Ee0qX/FAK4LxiZKQHx3Lgvi9+8F27jlh9Sd5zN16SPO1D+K8jq6uwK+3fd/xd8qF6elzx8WKR5fJ993wOVLGZ+QmuT+jK+jHoMTKwvXt0tv4s+r74ykfY9B+cAw7pMPkKwvaSPY94cM3RqWOQ99g3oPCCmrMrWNk7dq1FKhy2Ac5MHO+JCY8zwTmP+j2SRT6jh0q/+YktcsFlG/P2p/RpU/k4BrHtu8WAMpntZvcL4rB35ckvfeRXy7Lv54IrDoduMjDcok6IXZ5bHvLpuLPGrsgZreNgLz+pOtlt5/MCya+su172it17gRtRrGnSPvOOm42jfnMG/O0f/gOXENI7Lo+3LbKxR/j1nfLNxJj0H58DLGf1ffAh/88Cfw01o/WAoGaM2eOXH/99VxBNYZ7EhjyTo6sOiCvXNk8UMWPU06fmOpE1daoL1A2z6aNPqxJdqR6Fdo1Uu3rizZjokL5VH7FtlN5NlV9grJl3TxF4fEvqK/JSG/43LJpls9wJZMQOuQbH4Elsg29il/hre+Lp5EYi9q396vmtQfzHtSY28bwPajGyDp4jZwANnnlyuaBKn6sbVxA3lf+ZX2Bsnk27fRhhEalRcKjBCsSIpewfCRkoGrbbp5NVZ+gbFknr9Txz6kf4UmvdG7ZNNdnsJI0K4SsNi16G39m/YBkPAHlYixq396vmtcefI+Z85skSpF98PKX0FkHvOyJkvZRvr0sP9Y26ptXbdj2lgfJ/WoxJOm9j/xy2f6DicC+tYf9zs7OuLyDLz/bf/BqOHoxnDuuNsiLferJKnNM4Da//UReVvvIKzz+Wds2TpuglG/QDJ95Y25AmWT5hG+X3safWd9gx2MoE2NR+8n9ZlyrfQUFqjJ5Bw95Zhle9oDbeW65vDyAtDLtZflxt0NfagK26+PERnpwcvt8NxqDS299FJXL8q/QE4Y7oWXc5gHeCabYv7bEuGb1BQR55sGE/DEBOe2H7xsVt2/5yCyXte1i+8JAlvENmuETWXljbtKtA2yPkfe9l6y23HjdfUNefZNuWvWlKbwxpttr/bXaN1CgCKlAcjXTKlozQfRN7IT0HgoUIQ0R3GZJr55aQfMESouS91UzIfWFAkUIIaSWUKAIIYTUEgoUIYSQWkKB6gXJJ2WywPsIDXzWopa4cfeuHxg3+8EkDZ640m/c2+PVLAritZ/2KjyezaC/nQdl43XLteJY9hf62zGuJxSoylQ5AfvrSeubeHrRDwiCo1Be0WoaOfFqcerrSbS/nQdl4+1v/WolHItmQIGqSPw5AjOxmqe73Ffh9omatQ3SeZlfBFpiJo+f2rIn3ywfeb6tPGWJz1WU+VJWb6zId+My+6Hv0F26H8n85H6ZY2BjfwgyByViuUNelK9BDGZM7M/AACsvFaddD424fbH3g+3iL3R1fZo05Sd1TK2yUZpvnH3lQp86NmCX8fSh0S+LreQj6xzxnWv+tEQMzvhlf2Frli+SBwWqMuEF4b2OkOebULO2gZuXvHjiFUaZSRX10xdBlo9i375YrYvTWYWUiTUuo3fSE6Wu4+uHL5asmH3HwCLzq4wcmiBQ6G90PPV4xfGk8qyYkJcWl6z+Yzv0FX6gU1fV20XHJ6jrP6YZ46dBXla55H52P/PaTtIMHzFu7G55XxpcZ49fFJved8eiKB7iQoGqjHsCKvRFoS4SbfaJW7QNivKM38DMdegHF44qlyiU5aOMb188ebEX+VNgrMKM+IIHtq+sfuTEUngMLAoEKn7FG1s8ARXnx+TF3EgeKFs+bzsZczC8dhmQVT+k1Djn+SibZ9MMHwpv7L5zrdHrKK99ny9SBAWqMs4JiJM+8WrO5JXZBmXzyhNMoPYrRJ+PMr6L4rH3y/gDuGBRTpVPiES6fn4/rP1Sx8AmK91B+c2dV4ry82JuKA+ULV9m28ZNz6lTepybkWfTBB+ZsQckz7WAateRv5zPP8mGAlUZ5wTEiW9mKWxHeXY5ezt4RRVNbJl1QFDW/+o8DyMCZtvno4xvN568/fKx4mK1v6A1wPUN0v3wjlupY5BETxiOQHa5BZWvXAEqynfHJBEbdp28KB7fWOb0P9HPrG2fT+COUVZ97JYd5+R+/u257Ho2vfaRGbsB45OXVmX8bGxfKEOxyoMCVRnfCRku+zO/SNKpoy+Qojohvi+JTFygNnYs1unv8wGy0i0wMSA/uDDd+Jz9Ev40upzTz4SvjH7kjltWuttOTPCqNqzXqgnDHpNePSSBpDLnTda2wnt8nDLOfvr4+9r3lXN9hvVy2krv2/TWh1U/89wxZ4AvTVFq/FRK5pjZ/lt0vg0QKFD9GFwA9nVDCCEDCQpUv4WvvgghAxsKFCGEkFpCgSKEEFJLKFCEEEJqCQWKEEJILaFAEUIIqSUUKEIIIbWEAkUIIaSWUKAIIYTUEgoUIYSQWkKBIoQQUksoUIQQQmoJBYoQQkgtyRKo5cuXy8/u/RkFihBCSHugQBFCCKkl373ku/K9y74n119/vaxYsUJWrlwpY24dIzfcdIPcfMvNFChCCCHtASuoOXPmaIGaOGkiV1CEEELqgbnFN278OLnzrjspUA2Bn37WP7lu/ZyzNt9PTTcbt80yv66LOr2Jrbf1DWX8NKutLFz/rW6vUex4sF3lxyl9fezNj1zOkK6OOo0RGeiYW3xXXnUlBapR4p9cdyaCkZ1KMFr9a7dV2nQnrEbpbX1DGT/NaisL13+r22uUZsTTgj7hPCt+JURIU+AKqjL2q0nfZAexCNI7OztkkF5pmbxw1WNd6DO6VBmdHouMLy0mq02D2461r6xDVwx8BPF1SmeHtQrDRBTFDHz13fS8ydCOR7WVit3kRQEEZbogvEFePFxhXqK+319HV5eTB6z2dBlkptuL+wisOtZxi8H54PoEab+pfhT20d4Gbiy+tq0yibQMP6n0ZFyJ/iTOC0JaBwTqsccek5E/GJkQqE2bNsnYO8dSoLLBRWwEIXnha2Gxbv3ZE1286nIFzhUhX5pNVpsB2e3EdYL9OD7tI5whsW3HHeDWD9qJyqVELSZVzvKTHauaHMN4gjr+8Xb37bYCkU/GHODzkdVe4DMdYxbwZcea148yfbS3Ucyqk8Jt2+1j0o//2KFc2bEnpHXgFt/VI6+Wn479aUKg8EHdxc8upkBlEr3/BMILOrK8CcIuZ096aj8x6fjSbBxfiXJZ7fjisfajPmVNwgX1U/uGvHLYbjTWqnk2ReVcn74YHfREbsrYdfP8lsnLqxPSsraBvV9GoAlpDlmfg+I3SRSREijfRZt3oacJXu0bcQvwpQW4k4Y9aVaNJ5x8Rtp9symqX6XdZtQpm2dTVC7Pp4fUCiSrbpW8vDqKlrYN7H0KFOk7KFCVwUVbdNvDTQ+EJH3rzOC7+LMmBMc3BDOKJ6sdN5503BDEzs7OjBjT5bNvE9k48ehX+8ZPXqxxuhbqyHdQJxLkPH+JPJuisbD3s2K0QDsmoESbef0oyrN95MSS27bZBsn9/Ft8WfXcPEJaBwWqMpgoii5aT7oWEnMrxha4MC2edT1pNmnfiQnO246qhfcvVFowMXniS0xwaZL1dUocZ069RDzuQxKZYxI+wOHzreMs4S/1QEZM/lg4+xnjGWONQ6LNvH4U59k+knmmLURi7yf7W9zHsF5m28Dex7av/4Q0HwpUL8DF79WOfkzyVXz/p/39cSd7m7y8mmKv1ghpMRSo3oBX1QPmYg1uHeWugvoJWpS8K4N2MJAEyr5rQEjroUARQgipJRQoQgghtYQCRQghpJZQoAghhNQSChQhhJBakiVQ/LJYQgghbYUCRQghpJZcdtll+vegrrn2Glm6dCl/8p0QQkg9uOOOO/QKasxtY/h7UIQQQuqDucX3yKOP6JXT2rVrKVCEEELajy1Qo38yWt/io0ARQghpO1xBEUIIqSUQqOnTp+uHJCZOmhi9B8WffCeEENJW8JPveIpv3PhxqQ/q8iffCSGEtI2sz0HxmyQIIYS0FQoUIYSQWkKBIoQQUksoUIQQQmoJBYoQQkgtoUARQgipJRQoQgghtYQCRQghpJZQoAghhNSSXgvUm2++SaPRaDRa040CRaPRaLRaGgWKRqPRaLU0ChSNRqPRamkUKBqNRqPV0ihQNBqNRqulUaBoNBqNVkujQNFoNBqtlkaBotFoNFotjQJFq7WRcvzbv/2bd/xcI+UoGk9SjrLnZZZRoGi1NoATkpZt48aNa0igfD5osZUZT45jsTVyXmYZBYpWazMTgS+PFlgVgfLl0QJrRKB8ebTAKFC0AW+cCIqNAtVco0A1xyhQtAFvnAiKjQLVXKNANccoULQBb5wIio0C1VyjQDXHKFC0AW+cCIqNAtVco0A1xyhQtAFvfTIR9FwrgwcNlmt77PRuGZFKq2LN8pNt9RIo9HeQDLJsRLevnG2oM0RGtXCMGrH2CBTGYIR0e/P6p1GgaAPemj8RpK3n2sEyYsQIGXxtj5VOgapmjth0j1AiVTTxUqAoUH6jQNFqbc2fCFzrkWsHY2JQE8Tga6UnSg+F5VpMsL6VAPLNKsEWoKDeiBGDZdDgwWplFq8kkgLYPKu1QOl9e+K1xm1Ed3Jf2ZBRGKPAx4gRQ2TQkBEyYog19hC8IaOs49R8q4dAxeMyZNQoPR7BmAZjM2pUfF4GY5auNyh1HMyYmvGzyupjYco2xyhQtAFvzZ8IHMPtvfDi7B5hi1B48ZoL11kJoGwkOMiLxC2oF4sR9vfdFVTPKHtCtMe4R0YNcSbdxGQaT7zaR3gcsJ2ckJtvdRAojFOi/9H4BGOTd15G45MQ8+SYmrLpY9E8o0DRBrw1fyJIWnyRKsMFHb2SxAWdXhkF+1XzWmP1E6jwVbk9kfrylAVjj3RXoKz9nlEyRE+0rZlIXWu/QOWNRzPzfMeieUaBog14a/5EYFv6Io1fjSKvigjl5bXG6idQZiKEoNgTn51nm5vu7ofC1G2EyqS3xvYdgbL3m28UKNqAt+ZPBJYlVkyBxSsqXMDxrTo8SBHfxgvKZd/io0BFEx9WP9HEGwhW+hadO1m6+8FtLjzI0urbe7D2C5QzTvo2nhmP/LHKv8Vn18s6Fs0zChRtwFvzJ4LYErf3jEWihQs6fNhBr6xckUG+WXXlCxLaQbl98yGJ8D0UM1FqwTLjZibkeIyCCTPtIzlJt9baL1DK7HFSwhyPhzs2vn0zvnnllGUci2YZBYo24K35E8HAs3oJVGssIXIttvYIVLb1Zd+baRQo2oC3Vk4EwStHv/nK19XqJFC+sTTmK19swa2ovlo9wdohUO44BU/umbS+63szjQJFG/DW7IlgIFqdBGogWDsEaiAaBYo24I0TQbFRoJprFKjmGAWKNuCNE0GxUaCaaxSo5hgFijbgjRNBsVGgmmsUqOYYBYo24A3gRKflWyMC5atPS1oZgfLVoyWNAkUb0IYTnFbOfOPnmq8ezW++8TPmK0/zm2/8ylplgYJRoGg0Go3WKjMC1dPTQ4Gi0Wg0Wn0MAvXMM8+kBGrnzp0UKBqNRqO1z4xALV++XNauXSubN2/2C5QtUhQoGo1Go7XaIFCLFi1KCNT27dspUDQajUZrr0GgFi9eLCtWrJB169bJSy+9pAVq165dsmfPHr9AGZGiQNFoNBqtVQaBevbZZxMC9corr1CgaDQajdZeg0AtWbJEVq5cKevXr5ctW7ZkC5QtUhQoGo1Go7XSjEA9//zzsmHDBtm6dau8+uqrsnv3btm7dy8Fikaj0Wjtse9e8l254sor5Oabb5bVq1fLtm3b5OmFT8vk7skyfcZ0ChSNRqPR2mPmKb5bbrlF7hl3TyRQN91yk3RP6U4LlC1SECga7ctf/rJ84QtfkLPPPluGDRsm5557rt7Hf+yffvrpctJJJ+l8pKH88OHD5bzzztPljj/++Ggb6RdccIGuh/JIg5111lm6DPLPP/98OeOMM7Sviy++OKqH/0hHfZTHf5RBni/udtqJJ57oTS8y9O+hhx5qm2HC6C927733yhX/c4Xccccd3vzrRl8nP/jhD7x5RXWLDH7h305z24NvtIG27DJXXXWVTJgwIUqzzRfzmDFjEnWyYs/qry8OGPbh10532ypjbruoCx9It8uZMUMbl11+WdQuHpCYO3eu3HrbrfpR84XPLJQJ903Q3yaRK1BYRcHwvXx//dd/re2v/uqvaPuYffGLX9RCgP+wz33uc9LV1aW3hwwZIp///Of1PraNaKGMWweiYsphH/VOOeUUbUjDf5RFOvK/+tWv6nQIHgQQ9U0ayv3Lv/yLbsP2Z9pCHtK/9rWvab+oa/vE9jHHHKP7N3r0aPnXf/1X+c53viOXX365FhfY17/+dW0HHHCANtMX2yf2TR/gGwafnZ2diTEsa4gNTzW1y+bNm9dv7Paf3i6jfzLam/fUU0/J1d+/WqZMmeLNz6tbxlAXPuw01yfaRgyIxS7zw2t+KDNmzIjSbPPFdf8D9yfqZPUtq0++OEz6pZdfKpd875KE5cXnM7dd1L3mmmt03PPnz9e2YMECuf6G62XsnWNl0uRJ8uPrfqy/4giff8L7Tyhz2+236QclIFD33X+f/izUoA9/+MMpgXJFCgJli5TPfBcbbWAYVjFYpRx22GHaMLnDkP6xj31Miw72sY1yEJNjjz1Wb1944YVRHeybcshHvcMPP1wb0pCHskhH/sknn6zTP/WpT2lD/TPPPFOn2T5tf0jHtmkHKxL4RznbJ7ZRFv2DUCFmlIG4nnbaadrQFsweB5jtE/umD/ANQ9kDDzwwqteIISa8omyX4TMp/cEwoWGSw6rPl4905KOcm1dUt4zddPNNcvfP7k6kYR/pZv+JJ56Qa0ZdI7NmzUqUyYrL5Ns+YG5f4A9+4d8u56sL88WRl96oue2a8UW6eeHz9NNPaxG75957ZMojU+Saa6/R3yCxatUq/Xj5jJkzZMytY/SDEosWLyovUO5KyjWfWNEGll100UXa/v3f/10bbq1BDGD/8R//oQ235TCpYxVz1FFH6Yket+yQju1vfvOb0bcfIw0GoYAZX5jwYbhlCLv00ku1felLX9L2wx/+UNu3v/1tbVg9wf7rv/5Lm/Fr2oaQwNAH/Ece0pGP/nzrW9/S9pWvfEUbbkvATD+NP+PHlDfxmnhMuj1mBx10UGK/rOE2H15Ftst8olVHm/r4VLnxphv1E2BuHtKQhzJuHiyvblnDqgG3oew07CPd7E+fPl2u+8l1eqVgl8lr2/UBc+OFP/iFf7ucry4M5Uf9aFRqPMw4+eo0Yr52kYY2IUJ4hPzxJx7XogUBg2DhVh9WUhAk3Na786475YmfP6FFCflYfeFRcy1QjYgUbd+zc845R5uZyLFKwAQMM3kQpU9/+tNaMD7+8Y/rW2TYRjq2cQvs1FNP1faNb3xDm/GP1QzM7JtyRqjMLTSIH8ysVEzbRkCMX9P20KFDtcEn6iMWpCMffs37X8b/CSecoM20C1GFGT+4pQczfTfxGD8mftgnP/nJxH5ZO/roo/Wjtu0yvJrtD4ZX55MfmuzNw20mvMmOn3Dw5efVzTP4+8n1P9Gv/m0zvvAfvk15Xxwog/dacFvLpGEbaa5f42vaL6cl6sAf/MJ/UV1j8GHyfnTdj3RdpPvqlxmbMu3CD1ZosB+P/rG+zbdx40bZtGmTFi2suiBUo68fLQ88+IAWJzxevmTpEhlz2xi59fZbY4EqI1K2+S4uGo0W2MEHH+xNLzLcbsSF2i7DByXrbngVfttPb9OPJfvyx08YL488+og3r6gurTVmvwjCk3r4Qlh8pRE+lPvaa6/pc8989mnZc8u0YOnv4rMFKkukjPmEikajpe2QQw7xphcZVpK4SNtlmDBotGYbPnxrDIKEJ/QgSriNByHCt0bg/MOj58/1PCcPTnzQL1DGfAJF2zcN77eMGH6snDn0MLnwlGPkv889Tb467Bidds7xR8jFnx+m7cqvnKkN6TCUO/eEI3Xe1047Tt8Ow62zSy65JHpfC4bbboceeqh+bwiGp+SMmfbxVB7ej0LZESNG6Ft52I7iU2n/+Z//qetjH3moj/eQ8J4Zypv3nvAfZXH7Dp+/wPtZV1xxhf7//e9/X/s0MaIcfCFuMx7mvTQ8qQcz71XdeOON2lAGtztN+UYMj+S/8847bTNMGDRasw2rI2MQHiNKsDfeeEML01tvvSVvv/22FqiJkyaqMq/L/wc7scCauUnfLQAAAABJRU5ErkJggg==

[pure-call]:data:image/png;base64,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