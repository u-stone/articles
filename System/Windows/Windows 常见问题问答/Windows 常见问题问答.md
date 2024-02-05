[TOC]

# dll与C++运行时库

有一个cross-dll的问题，是说在一个dll中new一个内存段，能不能在另一个dll中delete？

这个问题要看具体的环境。如果编译这些dll的时候使用的是/mt，那就是说将部分的运行时库编译到了目标dll中，同时使用静态的运行时库负责堆的分配，这样的话，每个dll中都有一个对分配管理器，当然是不能cross-dll管理内存了；但是如果使用的是/md的方式编译这些dll，由于大家使用的都是dll版本的运行时库，是同一个堆分配器，那么就可以以crosll-dll的方式分配内存了。
  
参考下面的描述：
  
/MD makes your application require the C Runtime Library DLLs, e.g. ucrtbase.dll and vcruntime140.dll. /MT embeds the required parts of the C Runtime Library into your output file.

/MD is generally better: it makes smaller files, the processes consume less RAM (because many processes can use the same C Runtime Library DLLs mapped to memory), and Microsoft can update the DLLs to fix bugs.

However, you must use /MT if you are making an installer bootstrapper that needs to work even on computers in which the correct version of the C Runtime Library has not been installed yet. I suppose you could also use /MT if you were making some kind of plug-in and needed to isolate its CRT state from an ill-behaving host application.
  
参考：

- [What is the difference between Multi-Threaded DLL (/MD) and Multi-Threaded Runtime (/MT) libraries?](https://social.msdn.microsoft.com/Forums/en-US/52493674-7f78-4698-adc4-0800e3362681/what-is-the-difference-between-multithreaded-dll-md-and-multithreaded-runtime-mt-libraries?forum=msbuild)，  
- [/MD, /MT, /LD (Use Run-Time Library)](https://docs.microsoft.com/en-us/cpp/build/reference/md-mt-ld-use-run-time-library?view=vs-2019)，
- [关于Cross-Dll问题（在不同的模块之间申请和释放内存)](https://blog.csdn.net/pxhero2012/article/details/8974370)，
- [CRT Library Features](https://docs.microsoft.com/en-us/cpp/c-runtime-library/crt-library-features?view=vs-2019), 
- [Do (statically linked) DLLs use a different heap than the main program?](https://stackoverflow.com/questions/10820114/do-statically-linked-dlls-use-a-different-heap-than-the-main-program)

# dll hell是什么

参考 [I keep hearing about DLL hell - what is this? [closed]](https://stackoverflow.com/questions/1379287/i-keep-hearing-about-dll-hell-what-is-this)
就是说多个程序共享一个dll，但是各自又需要升级版本，在不同程序之间无法区分应该使用哪一个版本的dll。

# Windows上的内存管理：

- 虚拟内存：管理大型对象数组和数据结构数组
- 内存映射文件：大型数据流，一般是文件；以及在同一个机器上的多进程间共享数据
- 堆：最适合管理大量小对象
- 参考 `msdn` 上这篇文章了解更多内存申请函数的差异 [Comparing Memory Allocation Methods](https://docs.microsoft.com/en-us/windows/win32/memory/comparing-memory-allocation-methods)

# SEH的实现原理

TODO: 参考《Windows核心编程》第五版的SEH那一章，关于SEH展开的描述

# IOCP了解一下

# shared_ptr是不是线程安全的？
答：是“半”线程安全的，shared_ptr中只有控制部分使用了atomic变量，访问资源部分不是线程安全的。参考 [Atomic Smart Pointers](https://www.modernescpp.com/index.php/atomic-smart-pointers)

测试下面的代码发现，没有问题

```cpp
std::shared_ptr<int> ptr = std::make_shared<int>(2011);

for (auto i= 0; i<10; i++){
   std::thread([ptr]{                           (1)
     std::shared_ptr<int> localPtr(ptr);        (2)
     localPtr= std::make_shared<int>(2014);     (3)
    }).detach(); 
}
```

但是，下面的代码就会在指针析构的时候崩溃

```cpp
std::shared_ptr<int> ptr = std::make_shared<int>(2011);  

for (auto i= 0; i<10; i++){
   std::thread([&ptr]{                         (1)
     ptr= std::make_shared<int>(2014);         (2)
   }).detach(); 
}
```

即使为指针ptr的加上原子操作也不行:

```cpp
std::shared_ptr<int> ptr = std::make_shared<int>(2011);

for (auto i =0;i<10;i++){
   std::thread([&ptr]{ 
     auto localPtr= std::make_shared<int>(2014);
     std::atomic_store(&ptr, localPtr);            (1)
   }).detach(); 
}
```

**看不懂为什么？？？**
测试发现，在VC++环境下，新建一个console程序，main函数中只写入上面的函数，并不会crash。但是在xcode中，写一个类似的console程序会crash。原因猜测是for循环外层创建的ptr在退出主线程的main之后被析构了，for循环内部的ptr被重新赋值，但是对应的地址被释放了。

# 如何拦截其他进程的窗口消息

用的是钩子，但是钩子的应用比较广，还有使用钩子和反钩子的攻防技术。参考 
- [Windows 反消息钩子（1）](https://blog.csdn.net/songjinshi/article/details/7808624), 
- [Windows 反消息钩子（2）](https://blog.csdn.net/songjinshi/article/details/7808561)；
- 看雪上一篇 [[原创]关于Windows消息钩子的理解与测试项目（附源码）](https://bbs.pediy.com/thread-222315.htm)。

还有人提到 `inline hook` 和 `API hook`，以及替换 `IAT（import address table）` 中函数入口的技术。

- [Nettitude Blog Windows Inline Function Hooking](https://blog.nettitude.com/uk/windows-inline-function-hooking)
- [Windows API Hooking](https://ired.team/offensive-security/code-injection-process-injection/how-to-hook-windows-api-using-c++)，

找到一个关于进程注入的合集文章： **[Code & Process Injection](https://ired.team/offensive-security/code-injection-process-injection)**

# win32窗口的子类化，超类化是如何实现的。

- [Subclassing Controls](https://learn.microsoft.com/en-us/windows/win32/controls/subclassing-overview)。
- [窗口的子类化与超类化](http://www.cppblog.com/bigsml/archive/2007/08/24/30780.aspx)

子类化就是使用SetWindowLong替换WinProc，通过在新的WinProc中处理自己关心的消息，将不关心的转发给老的WinProc来达到扩展功能的效果。
超类化是创建并注册一个新的WNDCLASS，得到一个新的窗口类，同时使用老的窗口过程WinProc。
> 故超类化只能改变自己创建的窗口的特征，而不能用于由Windows创建的窗口（如对话框上的按钮就不能进行超类化） 。而子类化是实例级别上的，只要能获得窗口的实例，就可对其子类化，这是唯一的子类化对于超类化的优势。另外，凡是子类化可实现的，超类化都可实现，不过超类化用起来较麻烦。 
- 子类化修改窗口过程函数,  超类化修改窗口类(新的窗口类名)
- 子类化是在窗口实例级别上的，超类化是在窗口类（WNDCLASS）级别上的。
- 超类化可以完成比子类化更复杂的功能，在SDK范畴上，可以认为子类化是超类化的子集。
- 子类化只能改变窗口创建后的性质，对于窗口创建期间无能为力(无法截获ON_CREATE 事件)，而超类化可以实现；超类化不能用于Windows创建的窗口，子类化可以。 

子类化的目的是在不修改现有代码的前提下，扩展现有窗口的功能。它的思路很简单，就是将窗口过程地址修改为一个新函数地址，新的窗口过程函数处理自己感兴趣的消息，将其它消息传递给原窗口过程。通过子类化，我们不需要现有窗口的源代码，就可以定制窗口功能。

子类化可以分为实例子类化和全局子类化。实例子类化就是修改窗口实例的窗口过程地址，全局子类化就是修改窗口类的窗口过程地址。实例子类化只影响被修改的窗口。全局子类化会影响在修改之后，按照该窗口类创建的所有窗口。显然，全局子类化不会影响修改前已经创建的窗口。

子类化方法虽然是二十年前的概念，却很好地实践了面向对象技术的开闭原则（OCP：The Open-Closed Principle）：对扩展开放，对修改关闭。

超类化的概念更简单，就是读取现有窗口类的数据，保存窗口过程函数地址。对窗口类数据作必要的修改，设置新窗口过程，再换一个名称后登记一个新窗口类。新窗口类的窗口过程函数还是仅处理自己感兴趣的消息，而将其它消息传递给原窗口过程函数处理。使用GetClassInfo函数可以读取现有窗口类的数据。

ref: [Windows 窗口子类化和超类化](https://blog.csdn.net/xiaolongwang2010/article/details/10473151)

# 线程与消息队列：ref《Windows核心编程》第四版 26章

当一个线程第一次被建立时，系统假定线程不会被用于任何与用户相关的任务。这样可以减少线程对系统资源的要求。但是，一旦这个线程调用一个与图形用户界面有关的函数（例如检查它的消息队列或建立一个窗口），系统就会为该线程分配一些另外的资源，以便它能够执行与用户界面有关的任务。特别是，系统分配一个THREADINFO结构，并将这个数据结构与线程联系起来。这个THREADINFO结构包含一组成员变量，利用这组成员，线程可以认为它是在自己独占的环境中运行。THREADINFO是一个内部的、未公开的数据结构，用来指定线程的登记消息队列（posted-message queue）、发送消息队列（ send-message queue）、应答消息队列（ reply-message queue）、虚拟输入队列（virtualized-input queue）、唤醒标志 （wake flag）、以及用来描述线程局部输入状态的若干变量。