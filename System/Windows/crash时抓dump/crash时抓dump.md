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

以上主要在 MSDN [Process and Environment Control](https://docs.microsoft.com/en-us/cpp/c-runtime-library/process-and-environment-control?view=vs-2019)查找资料。

C\++信号处理 Signal Handling，也就是C\++中的程序中断机制。通过signal()函数处理。
ANSI标准中一共有六种，
1. SIGABRT Abnormal termination
2. SIGFPE Floating-point error，当浮点运算出错时由CRT调用，一般情况下不会生成，而是生成一个NaN或者无限大的数字，可以通过_controlfp_s函数打开这个异常。参考 [Floating-Point Exceptions](https://docs.microsoft.com/en-us/windows/win32/debug/floating-point-exceptions)
3. SIGILL Illegal instruction（*）
4. SIGINT CTRL+C signal 
5. SIGSEGV Illegal storage access （*）
6. SIGTERM Termination request （*）

*这部分可以参考 msdn [signal](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/signal?view=vs-2019)函数介绍*

标*的MSDN中提示说Windows NT不会生成，只是为了兼容ANSI；但是如果在主线程中设置了SIGSEGV信号函数，那么就会由CRT而不是SEH设置的SetUnhandledExceptionFilter()过滤函数来调用，并且有一个全局的变量_pxcptinfoptrs包含异常信息。如果是在其他线程的话，异常处理过程是由SEH的SetUnhandledExceptionFilter()过滤函数调用的。

除了函数之外，还有编译链接选项上的一些事情。CRT可以以MD（动态链接）和MT（静态链接）的方式编译进模块（exe/DLL）里面。参考：/MT、/MD编译选项，以及可能引起在不同堆中申请、释放内存的问题，/MD, /MT, /LD (Use Run-Time Library)。
MD的方式时推荐的，多个模块公用一个CRT的DLL库的方式；以MT的方式使用CRT的话，需要把函数写成static，并且使用/NODEFAULTLIB链接标记，链接到所有模块中，还需要每个模块中都注册CRT错误处理过程。

## 为什么调试器可以抓到所有崩溃?


## CRT, C++ STL, 系统API的两两关系

### abort() vs exit() vs terminate()

先说一下我电脑Win10上CRT的源码位置：C:\Program Files (x86)\Windows Kits\10\Source\10.0.19041.0\ucrt，注意不在VC++或者VS的路径下了。使用VSCode打开可以搜索想要查找的函数。

#### terminate

[terminate (CRT)](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/terminate-crt?view=vs-2019)的Remark中提到，terminate用于同C\++异常配合，在下面三种情况下会被调到：

1. 抛出一个C++异常，但是没有匹配的catch
2. 析构函数退栈的时候抛出的异常
3. 抛出异常之后，栈毁坏了

```cpp
/// 源码
extern "C" terminate_handler __cdecl set_terminate(
    terminate_handler const new_handler
    )
{
    __acrt_ptd* const ptd = __acrt_getptd();

    terminate_handler const old_handler = get_terminate_or_default(ptd);

    ptd->_terminate = new_handler;

    return old_handler;
}

extern "C" void __cdecl terminate()
{
    terminate_handler const handler = __acrt_getptd()->_terminate;
    if (handler)
    {
        // Note:  We cannot allow any exceptions to propagate from a user-
        // registered terminate handler, so if any structured exception escapes
        // the user handler we abort.
        __try
        {
            handler();
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            ; // Deliberately do nothing
        }
    }

    // If the terminate handler returned, faulted, or otherwise failed to end
    // execution, we will do it:
    abort();
}

```

terminate()默认会调用abort()，通过设置自定义终止函数(set_terminate),MSDN上一个例子：

```c
// exceptions_Unhandled_Exceptions.cpp
// compile with: /EHsc
#include <iostream>
using namespace std;
void term_func() {
   cout << "term_func was called by terminate." << endl;
   exit( -1 );
}
int main() {
   try
   {
      set_terminate( term_func );
      throw "Out of memory!"; // No catch handler for this exception
   }
   catch( int )
   {
      cout << "Integer exception raised." << endl;
   }
   return 0;
}

/// output:  term_func was called by terminate.
```

#### abort

[abort](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/abort?view=vs-2019)

abort不会把执行控制权交给调用者，而是检查abort信号处理函数，如果设置了SIGABRT信号就发起SIGABRT信号。
微软的实现中，debug版本的库会在发起SIGABRT信号前弹出一个错误消息；如果不想要提示，可以设置 [_set_abort_behavior](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/set-abort-behavior?view=vs-2019)，

```c
// crt_set_abort_behavior.c
// compile with: /TC
#include <stdlib.h>

int main()
{
   printf("Suppressing the abort message. If successful, this message"
          " will be the only output.\n");
   // Suppress the abort message
   _set_abort_behavior( 0, _WRITE_ABORT_MSG);
   abort();
}
```

弹出的提示框如图（消息内容根据VC的crt的版本有所不同）：

![abort-called][abort-call]

如果点击Abort，程序会立即退出并返回错误码3；如果点击Retry，程序会请求一个JIT调试器（如果设置了的话）；如果点击Ignore，程序会继续执行abort逻辑退出程序。

在retail和debug版本下，abort会检查用户有没有设置SIGABRT信号处理函数，有的话就调用。我们可以通过[signal](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/signal?view=vs-2019)来设置自定义的SIGABRT函数，这样有一个机会一些优雅的资源回收，错误报告，日志记录，或者生成一个dump（通过RaiseException触发）。如果没有设置的话，就会调用raise(SIGABRT)。

非debug版本的窗口程序和控制台程序中，abort会调用Windows错误报告服务，一般来说就是Dr.Waston窗口。用户可以设置[_set_abort_behavior](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/set-abort-behavior?view=vs-2019)来控制这个行为。如果没有调用Windows错误报告服务，那么就直接调用_exit （参考 [exit, _Exit, _exit](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/exit-exit-exit?view=vs-2019)） 结束进程并返回3，并把程序执行的控制权返回父进程或者操作系统。

```c
_set_abort_behavior(0, _CALL_REPORTFAULT);
```

关于这个crt函数在遇到dll的时候还有一个讨论（bug or 兼容性）：[The documentation of abort/_set_abort_behavior should mention legacy behavior](https://github.com/MicrosoftDocs/cpp-docs/issues/219)

```cpp
/// _set_abort_behavior 源码
extern "C" unsigned int __cdecl _set_abort_behavior(unsigned int flags, unsigned int mask)
{
    unsigned int oldflags = __abort_behavior;
    __abort_behavior = oldflags & (~mask) | flags & mask;
    return oldflags;
}

/// abort的源码，上面说了那么多，权当这个函数的解释了。
extern "C" void __cdecl abort()
{
    #ifdef _DEBUG
    if (__abort_behavior & _WRITE_ABORT_MSG)
    {
        __acrt_report_runtime_error(L"abort() has been called");
    }
    #endif


    /* Check if the user installed a handler for SIGABRT.
     * We need to read the user handler atomically in the case
     * another thread is aborting while we change the signal
     * handler.
     */
    __crt_signal_handler_t const sigabrt_action = __acrt_get_sigabrt_handler();
    if (sigabrt_action != SIG_DFL)
    {
        raise(SIGABRT);
    }

    /* If there is no user handler for SIGABRT or if the user
     * handler returns, then exit from the program anyway
     */

    if (__abort_behavior & _CALL_REPORTFAULT)
    {
        #if defined _M_ARM || defined _M_ARM64 || defined _UCRT_ENCLAVE_BUILD
        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
        #else
        if (IsProcessorFeaturePresent(PF_FASTFAIL_AVAILABLE))
            __fastfail(FAST_FAIL_FATAL_APP_EXIT);

        __acrt_call_reportfault(_CRT_DEBUGGER_ABORT, STATUS_FATAL_APP_EXIT, EXCEPTION_NONCONTINUABLE);
        #endif
    }

    /* If we don't want to call ReportFault, then we call _exit(3), which is the
     * same as invoking the default handler for SIGABRT
     */

    _exit(3);
}

```

#### exit

[exit, _Exit, _exit](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/exit-exit-exit?view=vs-2019)

找到源码看一下：

```cpp
extern "C" void __cdecl exit(int const return_code)
{
    common_exit(return_code, _crt_exit_full_cleanup, _crt_exit_terminate_process);
}

extern "C" void __cdecl _exit(int const return_code)
{
    common_exit(return_code, _crt_exit_no_cleanup, _crt_exit_terminate_process);
}

extern "C" void __cdecl _Exit(int const return_code)
{
    common_exit(return_code, _crt_exit_no_cleanup, _crt_exit_terminate_process);
}

extern "C" void __cdecl quick_exit(int const return_code)
{
    common_exit(return_code, _crt_exit_quick_cleanup, _crt_exit_terminate_process);
}

extern "C" void __cdecl _cexit()
{
    common_exit(0, _crt_exit_full_cleanup, _crt_exit_return_to_caller);
}

extern "C" void __cdecl _c_exit()
{
    common_exit(0, _crt_exit_no_cleanup, _crt_exit_return_to_caller);
}
```

调用的都是common_exit。

exit 总体上会：

1. 调用thread-local的析构函数 
2. 然后按照后入先出的方式调用注册的atexit和_onexit函数  （_execute_onexit_table）
3. 刷新所有文件缓存
4. 退出程序

_Exit和_exit只是退出进程，不做1，2，3的清理工作，只做4.

如果在函数中调用exit, _Exit, _exit，那么要注意手动释放自动生成的对象（非静态对象）和临时对象（编译器生成的对象，比如函数返回值）。

```cpp
void last_fn() {}
    struct SomeClass {} myInstance{};
    // ...
    myInstance.~SomeClass(); // explicit destructor call
    exit(0);
}
```

关于 [atexit](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/atexit?view=vs-2019)，[_onexit](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/onexit-onexit-m?view=vs-2019)，二者用法基本上是一样的，_onexit是微软的一个扩展，为了ANSI 的可移植性要使用atexit。

```cpp
// crt_atexit.c
#include <stdlib.h>
#include <stdio.h>

void fn1( void ), fn2( void ), fn3( void ), fn4( void );

int main( void )
{
   atexit( fn1 );
   atexit( fn2 );
   atexit( fn3 );
   atexit( fn4 );
   printf( "This is executed first.\n" );
}

void fn1()
{
   printf( "next.\n" );
}

void fn2()
{
   printf( "executed " );
}

void fn3()
{
   printf( "is " );
}

void fn4()
{
   printf( "This " );
}
```

正好StackOVerflow上也有一个类似的问题：[abort, terminate or exit?](https://stackoverflow.com/questions/2820285/abort-terminate-or-exit)

说，abort就像是异常终止程序，也就是说程序无法“挽救”了，并发起一个SIGABRT信号，不论你设置了什么处理函数，都会终止程序；exit是一个正常的退出，比如说遇到错误的输入，但这不是程序的错误。而terminate是报告一个无法处理的异常可以采取的最后一招。

### 还有 set_unexpected 

[unexpected (CRT)](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/unexpected-crt?view=vs-2019) 默认调用terminate，可以通过[set_unexpected (CRT)](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/set-unexpected-crt?view=vs-2019)来做自定义行为。

msdn上说这个函数的流程不与C++异常处理一起使用。

而且没有在上面的CRT目录下找到这个函数的实现，只有一个头文件 C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Tools\MSVC\14.16.27023\include\eh.h

[Why the function unexpected is not called?](https://stackoverflow.com/questions/25403585/why-the-function-unexpected-is-not-called)


```cpp
void my_terminate() {
    std::cerr << "my terminate handler";
    std::exit(0);
}

void my_unexpected() {
    std::cerr << "my unexpected handler";
    std::exit(EXIT_FAILURE);
}

void function() throw() // no exception in this example, but it could be another spec
{
    throw std::exception();
}

int main(int argc, char* argv[])
{
    std::set_terminate(my_terminate);
    std::set_unexpected(my_unexpected);
    try {
        function();
    } catch (const std::logic_error&) {
    }
    return 0;
}
```

我在MSVC上试了这个例子 没用!!!  有人说这是微软的一个bug： [Exception Handling - set_unexpected() not able to call](https://stackoverflow.com/questions/10056909/exception-handling-set-unexpected-not-able-to-call)，gcc是可以的。

### _set_error_mode

[_set_error_mode](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/set-error-mode?view=vs-2019), 控制错误信息的输出模式，使用标准输出，还是弹框。

## 进程内捕获异常 VS 进程外捕获异常

参考资料中提到的[google-breakpad](https://chromium.googlesource.com/breakpad/breakpad)介绍到，进程外异常才是安全的。

# 理一下思路

Windows上的崩溃，

# 测试代码

Windows上抓取minidump的代码可以参考 [这里](http://www.debuginfo.com/examples/effmdmpexamples.html)。

放在了[github](https://github.com/u-stone/articles/tree/master/System/Windows/crash%E6%97%B6%E6%8A%93dump)上 。

![invalid-parame-crash-dialog][invalid-parame-crash-dialog]

![pure-call][pure-call]

# 参考资料

一个被广泛使用的开源项目 [CrashRpt](http://crashrpt.sourceforge.net/)，文档中有异常和异常处理的一篇好文章：[About Exceptions and Exception Handling](http://crashrpt.sourceforge.net/docs/html/exception_handling.html), 国内2011年就有人[翻译](https://www.cnblogs.com/hdtianfu/archive/2011/12/27/2303113.html)了的。

谷歌开源项目[google-breakpad](https://chromium.googlesource.com/breakpad/breakpad)，docs中先看[getting_started_with_breakpad.md](https://chromium.googlesource.com/breakpad/breakpad/+/master/docs/getting_started_with_breakpad.md)这个概述文件。

张银奎老师[软件调试](https://book.douban.com/subject/3088353/)中第11章中断和异常管理，第12章 未处理异常和JIT调试。

[深入解析Windows操作系统：第6版（上册）](https://book.douban.com/subject/25844377/) 第三章系统机制中 3.1 陷阱分发。

[Matt Pietrek](https://en.wikipedia.org/wiki/Matt_Pietrek) 在 Microsoft Systems Journal中的一篇文章 [A Crash Course on the Depths of Win32™ Structured Exception Handling](https://web.archive.org/web/20040603050159/http://www.microsoft.com/msj/0197/exception/exception.aspx)

[Under The Hood - Matt Pietrek](https://docs.microsoft.com/en-us/archive/blogs/matt_pietrek/)

MSDN上的资料：

- [Structured Exception Handling](https://docs.microsoft.com/en-us/windows/win32/debug/structured-exception-handling),

- [Exception Handling Routines](https://docs.microsoft.com/en-us/cpp/c-runtime-library/exception-handling-routines?view=vs-2019)

- [Unhandled C++ exceptions](https://docs.microsoft.com/en-us/cpp/cpp/unhandled-cpp-exceptions?view=vs-2019)

- [Handle structured exceptions in C++](https://docs.microsoft.com/en-us/cpp/cpp/exception-handling-differences?view=vs-2019)

- [C runtime Library Reference](https://docs.microsoft.com/en-us/cpp/c-runtime-library/c-run-time-library-reference?view=vs-2019)

- [Compatibility](https://docs.microsoft.com/en-us/cpp/c-runtime-library/compatibility?view=vs-2019)

[SetUnhandledExceptionFilter拦不住的崩溃](https://blog.csdn.net/limiteee/article/details/8472179) 介绍了一种不被CRT替换掉自定义异常过滤函数的方法。

[Why SetUnhandledExceptionFilter cannot capture some exception but AddVectoredExceptionHandler can do](https://stackoverflow.com/questions/19656946/why-setunhandledexceptionfilter-cannot-capture-some-exception-but-addvectoredexc) 介绍了VEH比SEH高级的地方。

[Getting an exception call stack from the catch block (C++) ](https://docs.microsoft.com/en-us/archive/blogs/slavao/getting-an-exception-call-stack-from-the-catch-block-c) MSDN上的一篇blog，介绍了C++的try-catch抓到一个异常之后如何获取到call stack。有点高深，没看懂。

[windows下捕获dump](https://www.cnblogs.com/cswuyg/p/3207576.html)  有一个写好的例子。

[网游客户端弹出个“Runtime Error”不产生dump文件的解决办法](http://www.cppblog.com/lai3d/archive/2011/08/25/154358.html)

[C++异常处理类与自定义异常处理类](https://blog.csdn.net/makenothing/article/details/43273137)

[Integrating Crash Reporting into Your Application - A Beginners Tutorial](https://www.codeproject.com/Articles/308634/Integrating-Crash-Reporting-into-Your-Application)，
[Add Crash Reporting to Your Applications with theCrashRpt Library](https://www.codeproject.com/Articles/3497/Add-Crash-Reporting-to-Your-Applications-with-the)

[What Every Computer Programmer Should Know About Windows API, CRT, and the Standard C++ Library](https://www.codeproject.com/Articles/22642/What-Every-Computer-Programmer-Should-Know-About-W)

[Crt, API, STL, MFC, ATL之间的关系](https://blog.csdn.net/acelit/article/details/58584706)



Raymond的一篇文章 [Disabling the program crash dialog](https://devblogs.microsoft.com/oldnewthing/20040727-00/?p=38323)






[invalid-parame-crash-dialog]:data:image/png;base64,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

[pure-call]:data:image/png;base64,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

[abort-call]:data:image/png;base64,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