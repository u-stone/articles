@[toc]

Intel 64和IA-32架构软件开发者手册 Vol 3A部分内容学习摘要。

# Chapter 8 多核处理器管理

Intel 32位IA-32处理器支持系统内存中的锁定原子操作。这类操作通常是用来管理共享数据结构，比如信号量，段描述符，系统段或者页表，有多个处理器要同步修改相同的位或者标记。处理器使用三个独立的机制来实现锁定原子操作：

1. 可靠的原子操作。
2. 总线锁定。
3. 缓存一致性协议，可以保证缓存的数据结构以原子操作的方式实现；这个机制在Pentium 4, Intel Xeon, and P6家族处理器中就出现了。

## Guaranteed Atomic Operations （可靠的原子操作）

Intel486处理器保证下面的操作一定是原子的：

1. 读/写一个字节
2. 在16bit边界的时候，读/写一个word
3. 在32bit边界的时候，读/写一个double-word （双字）

奔腾处理器及其后继者，还会保证下面内存操作是原子的：

1. 在64bit边界的时候，读/写一个quad-word （4字）
2. 通过32bit数据总线访问一个16bit的未缓存内存地址

P6家族及其后继者，还保证下面的内存操作是原子的：

- 在缓存行中访问一个未对齐的16/32/64位缓存的内存

## Bus Locking (总线锁定)

Intel 64和IA-32处理器提供了一个LOCK#信号，用在某些关键内存操作的时候自动触发锁定系统总线或者等效的链路。当这个信号触发的时候，来自其他处理器或者总线代理对该总线的控制将被阻塞。软件层面也可以通过在一个指令前放置LOCK来指定一些场景。

Intel 386、486和奔腾处理器中显式的锁定指令将会触发锁定信号；不过，P6以及较新的处理器家族，如果处理器中缓存了待访问的内存区域，一般不会触发LOCK信号，锁定一般发生在处理器缓存中。

### 自动锁定

下列操作执行的时候，处理器会自动跟一个LOCK语义

1. 执行关联内存的XCHG指令
2. 设置Task-State Segment的B(busy)标记的时候
3. 更新段描述符的时候
4. Intel 386处理器不论段描述符是否“干净”都会设置为“已访问”状态；奔腾4、Intel Xeon、P6家族、奔腾和Intel 486处理器只在段描述符“不干净”的时候才会设置
5. 更新页目录和页表项的时候
6. 确认中断 Acknowledging interrupts

### 软件控制

为了显式地使用LOCK语义，软件可以在下列指令修改内存地址时使用LOCK前缀修饰。当LOCK前缀用于其他指令，或者没有操作数是内存地址的时候，处理器会产生一个无效操作码异常（invalid-opcode exception #UD）。

- 位的test和modify指令（BTS、BTR和BTC）
- 交换指令（XADD、CMPXCHG和CMPXCHG8B）
- XCHG会自动添加LOCK前缀
- 下面的单操作数算数/逻辑指令：INC、DEC、NOT和NEG
- 下面的双操作数算数/逻辑指令：ADD、ADC、SUB、SBB、AND、OR和XOR

总线锁定的完整性不受内存域的影响。但是LOCK语义之后会跟足够多的总线周期来更新全部操作数。不过还是建议锁定的访问地址按照自然边界对齐，以获得更好的性能：

- 8bit访问的时候可按照任意边界对齐
- 锁定字访问的时候按照16bit边界对齐
- 锁定双字访问的时候按照32bit边界对齐
- 锁定四字访问的时候按照64bit边界对齐

对于Intel486和奔腾处理器而言，LOCK操作期间总是会在总线上触发一个LOCK#信号，即便锁定的内存区域在处理器的缓存中。

## 内存顺序

内存顺序（memory ordering）是指处理器通过系统总线读取（load）和写入（store）系统内存的顺序。Intel 64和IA-32架构支持多种内存顺序模型，具体要看实现。比如Intel386处理器强制使用程序顺序program ordering，也就是强类型顺序（strong ordering），任何情况下读写顺序都是按照指令流中的顺序来的。

为了优化指令执行的性能，IA-32架构部分地放弃强类型顺序，而在奔腾4，Intel Xeon，P6家族处理器中使用处理器顺序（processor ordering），处理器顺序的变种（内存顺序模型）比如允许读操作放在缓存写的前面，这些变种的目的是为了增加指令执行的速度，同时保证内存一致性，即使是在多处理器系统中。

### Intel® Pentium® and Intel486™ 处理器中的内存顺序
虽然Pentium and Intel486处理器遵循处理器顺序内存模型，但是大多数情况下他们以strongly-ordered的处理器方式运行。系统总线上读和写的顺序一般是按照程序顺序program-order进行的，除了下面的情况；当所有缓冲写(buffered writes)操作都缓存命中时，因此不会指向读缺失所访问的同一个地址，在系统总线上，读缺失（read miss）操作就允许放在缓冲写操作前面。note: 当缓冲的写操作都成功命中，而写操作缺失，那就意味着，读与写操作的是不同的内存地址，这个时候，把读写操作乱序执行一下是不会带来执行逻辑上的错误的。

在I/O操作中，读和写都是按照程序顺序进行的。

旨在可以正确地运行在处理器顺序处理器中的程序，不应该依赖奔腾或者Intel486处理器这类较强顺序内存模型硬件。而应该保证，在多个处理器中用于控制并行执行的共享变量的访问操作，要显式地通过适当地加锁操作和序列化操作来达到遵循程序顺序的要求，

### P6和最近处理器家族的内存模型

Intel Core 2 Duo, Intel Atom, Intel Core Duo, Pentium 4, and P6家族的处理器使用的是处理器顺序的内存顺序模型（processor-ordered memory-ordering model），其拥有下面的特性。

单处理器系统中（单处理器指的是单逻辑处理器，比如一个物理CPU如果是多核的CPU或者是Intel的超线程技术的CPU就是多处理器系统）：

- 读操作之间重排
- 写操作之间重排
- 写操作之间不重排，除非
  - 流式写操作执行非临时性的移动操作（MOVNTI, MOVNTQ, MOVNTDQ, MOVNTPS, and MOVNTPD）
  - 字符串操作
- 写入内存的操作不会与CLFLUSH指令重排；如果一个CLFLUSHOPT指令刷新的缓存行不是某个写入指令的地址，那么写该地址的指令可以与该CLFLUSHOPT重排。CLFLUSH指令之间不重排；CLFLUSHOPT访问的缓存行地址不同的话，它们之间可以重排；CLFLUSH和CLFLUSHOPT访问的缓存行不同的话，可以重排
- 读操作可以与一个之前的且目标地址不同的写操作重排，但是不能与目标地址相同的写操作重排
- 读/写操作不能与I/O、锁定、序列化指令重排
- 读操作不能提前置于LFENCE、MFENCE指令之前
- 写操作、CLFLUSH、CLFLUSHOPT不能提前置于LFENCE、SFENCE、MFENCE指令之前
- LFENCE不能重排置于的读操作之前
- SFENCE不能重排置于CLFLUSH和CLFLUSHOPT之前
- MFENCE不能重排置于读、写、CLFLUSH和CLFLUSHOPT之前

```c
汇编指令解释：
LFENCE：Load Fence
MFENCE：Memory Fence
SFENCE：Store Fence
CLFLUSH：Cache Line Flush
CLFLUSHOPT：Cache Line Flush Optimized
```

多处理器系统中：

- 独立的处理器使用在单个逻辑处理器系统中相同的顺序
- 单个逻辑处理器的写操作顺序将会被所有其他处理器观察到
- 单个逻辑处理器的写操作不与其他逻辑处理器的写操作重排
- 内存排序遵循传递可见性
- 除执行存储的处理器外，任何两个存储的顺序都是一致的
- 锁定指令有一个总的顺序



# 参考

1. [Intel® 64 and IA-32 Architectures Software Developer’s Manual Volume 3 (3A, 3B, 3C & 3D): System Programming Guide](https://software.intel.com/en-us/articles/intel-sdm)
2. [About self-modifying code](https://www.reddit.com/r/learnprogramming/comments/o672x/about_selfmodifying_code/) 想要写一个自修改代码，简单地说是不可能的，因为现代的操作系统一般将代码放在.text区域，而这个区域是只读的，任何打算改动这个区域的代码都会触发一个错误。可以修改的数据放在了.data区域。[C (programming language): Can you write a C program to demonstrate a self modifying code?](https://www.quora.com/C-programming-language-Can-you-write-a-C-program-to-demonstrate-a-self-modifying-code)、[How to write self-modifying code in x86 assembly](https://stackoverflow.com/questions/4812869/how-to-write-self-modifying-code-in-x86-assembly)(给了一个Linux下的例子，使用mpretect修改代码的区域，使得)是几个有趣的类似的问题和回答。维基百科词条 [Self-modifying code](https://en.wikipedia.org/wiki/Self-modifying_code)，不过Cross-modifying code就没有那么多资料了，Intel的论坛上有一个[Cross processor code modification](https://software.intel.com/en-us/forums/intel-isa-extensions/topic/624413)问题。
3. 

