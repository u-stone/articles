[TOC]

# 基础知识

汇编语言不同于、比如C/C++这类高级语言，它是一门与机器（CPU）相关的语言，比如有Intel的汇编语法，也有IBM的汇编语言，而且两家的语言在语法和指令集上不尽相同。

## IA-32处理器体系结构

有三种基本的操作模式：

1. 保护模式 Protected Mode，另外一种虚拟8086模式（Virtual-8086 Mode）是它的一种特例
2. 实地址模式 Real-address Mode
3. 系统管理模式 SMM， System Management Mode

其中保护模式最简单最强大，其他模式只在程序需要直接访问系统硬件的时候使用。

## 寄存器

Intel的CPU都有下面这些常见的寄存器

### 基础寄存器

1. 8个32位通用寄存器 EAX EBX ECX EDX EBP ESP ESI EDI
2. 6个16位段寄存器 CS SS DS ES FS GS
3. 1个32位指令指针寄存器 EIP
4. 1个32位处理器状态标志寄存器 EFLAGS

### 系统寄存器

1. 中断描述符表寄存器 IDTR，Interrupt Descriptor Table Register
2. 全局描述符表寄存器 GDTR，Global Descriptor Table Register
3. 局部描述符表寄存器 LDTR，Local Descriptor Table Register
4. 任务寄存器 Task Register
5. 调试寄存器 Debug Register
6. 控制寄存器 Control Register
7. 模型专用寄存器 Model-Specific Register

### 浮点寄存器

毕竟现在的CPU早就拥有独立的浮点运算单元FPU了

1. 8个80位的浮点寄存器：ST(0),...ST(7)。
2. 1个48位的FPU指令指针寄存器
3. 1个FPU数据指针寄存器
4. 1个80位的操作码寄存器
5. 3个16位控制寄存器

### 其他寄存器

比如为了支持多媒体程序设计的寄存器

1. MMX指令集使用的8个64位寄存器
2. SIMD，single-instruction，multiple-data 使用的8个128位XMM寄存器

## 过程调用中的关键指令

调用一个过程主要涉及到传参数，返回结果，跳转到新的执行地址，这些指令常见的有：

### 控制转移指令

转移指令又分为两种，无条件转移和条件转移

#### 无条件转移指令

比如JMP就是一个常见的无条件转移指令，格式: JMP 目的地址

CPU执行JMP指令时，标识目的地址的代码标号将由汇编器翻译成偏移地址，目标标号的偏移地址被装入指令指针中，CPU立即在新的地址继续执行指令。

#### 条件转移指令

条件转移指令很多，比如LOOP；LOOP的执行分两步：
1. 首先ECX减1，接着与0比较；
2. 然后开始判断
    - 如果ECX不等于0则跳转到目的地址处；
    - 如果ECX等于0，则不发生跳转，继续执行LOOP之后的指令处。

IA-32指令集中没有高级的逻辑结构，无论多么复杂的结构，都是用比较和跳转指令完成的。

### CALL和RET指令

CALL指令指示处理器在新的内存地址执行命令，以实现过程调用；RET指令使处理器返回到程序中过程被调用的地方继续执行。底层实现是，CALL指令把返回地址压入堆栈并把被调用过程的地址复制到指令指针寄存器，而程序返回时，RET指令从堆栈中弹出返回地址并送到指令指针寄存器中。
