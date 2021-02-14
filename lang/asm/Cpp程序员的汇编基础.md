[toc]

# gcc

## 一个最简单的例子

运行环境在Mac上，Mac上的代码，可以认为就是x64的代码：

```c
int t1(int a, int b) {
    int r = a + b;
    return r;
}

int main() {
    t1(2, 3);
    return 0;
}
```

保存文件，使用命令编译：

`gcc -o cpp-asm ./cpp-asm.cpp`

使用Hopper打开编译好的二进制文件之后：

t1反汇编的代码：

```cpp
        ; Section __text
        ; Range: [0x100000f60; 0x100000fab[ (75 bytes)
        ; File offset : [3936; 4011[ (75 bytes)
        ; Flags: 0x80000400
        ;   S_REGULAR
        ;   S_ATTR_PURE_INSTRUCTIONS
        ;   S_ATTR_SOME_INSTRUCTIONS



        ; ================ B E G I N N I N G   O F   P R O C E D U R E ================

        ; Variables:
        ;    var_4: -4
        ;    var_8: -8
        ;    var_C: -12


                     __Z2t1ii:        // t1(int, int)
0000000100000f60         push       rbp                                         ; CODE XREF=_main+25
0000000100000f61         mov        rbp, rsp
0000000100000f64         mov        dword [rbp+var_4], edi
0000000100000f67         mov        dword [rbp+var_8], esi
0000000100000f6a         mov        eax, dword [rbp+var_4]
0000000100000f6d         add        eax, dword [rbp+var_8]
0000000100000f70         mov        dword [rbp+var_C], eax
0000000100000f73         mov        eax, dword [rbp+var_C]
0000000100000f76         pop        rbp
0000000100000f77         ret
```

main函数的反汇编代码：

```cpp
        ; ================ B E G I N N I N G   O F   P R O C E D U R E ================

        ; Variables:
        ;    var_4: -4
        ;    var_8: -8


                     _main:
0000000100000f80         push       rbp
0000000100000f81         mov        rbp, rsp
0000000100000f84         sub        rsp, 0x10
0000000100000f88         mov        dword [rbp+var_4], 0x0
0000000100000f8f         mov        edi, 0x2                ; 第一个参数传给edi
0000000100000f94         mov        esi, 0x3                ; 第二个参数传给esi
0000000100000f99         call       __Z2t1ii                ; t1(int, int)
0000000100000f9e         xor        ecx, ecx
0000000100000fa0         mov        dword [rbp+var_8], eax
0000000100000fa3         mov        eax, ecx
0000000100000fa5         add        rsp, 0x10
0000000100000fa9         pop        rbp
0000000100000faa         ret
```

观察几个点：

## 函数调用时候的标准化步骤

main函数调用中压栈：

```cpp
push rbp
mov  rbp rsp
```

main函数调用状态出栈：

```cpp
pop rbp
```

t1函数也能看到类似的代码。

rsp和rbp配合用来对栈上的值进行寻址，在一个函数中一般会使用 `[rbp+xxx]` 的方式来对函数内的局部变量进行寻址，xxx就是一个常量偏移值，比如-4, -8, -C这样的(由于栈上的地址是像小的方向变化的，所以局部变量地址都是rbp减去一个常量得到的值)。

`[constant + reg]` / `constant[reg]`的方式就是汇编语言中的变址寻址方式。

## 参数传递

main函数中，参数2，3传给t1的方式：

```
0000000100000f8f         mov        edi, 0x2
0000000100000f94         mov        esi, 0x3
0000000100000f99         call       __Z2t1ii                                    ; t1(int, int)
```

前2个参数分辨放在了edi、esi中，然后就是调用t1函数（__Z2t1ii）。如果有更多的参数可以看一下：

```
int t2(int c, int d, int e, int f) {
    return c + d + e + f;
}
```

t2代码的反汇编结果：

```


        ; ================ B E G I N N I N G   O F   P R O C E D U R E ================

        ; Variables:
        ;    var_4: -4
        ;    var_8: -8
        ;    var_C: -12
        ;    var_10: -16


                     __Z2t2iiii:        // t2(int, int, int, int)
0000000100000f50         push       rbp                                         ; CODE XREF=_main+53
0000000100000f51         mov        rbp, rsp
0000000100000f54         mov        dword [rbp+var_4], edi      ; 第一个参数
0000000100000f57         mov        dword [rbp+var_8], esi      ; 第二个参数
0000000100000f5a         mov        dword [rbp+var_C], edx      ; 第三个参数
0000000100000f5d         mov        dword [rbp+var_10], ecx     ; 第四个参数
0000000100000f60         mov        eax, dword [rbp+var_4]      ; 开始累加
0000000100000f63         add        eax, dword [rbp+var_8]
0000000100000f66         add        eax, dword [rbp+var_C]
0000000100000f69         add        eax, dword [rbp+var_10]
0000000100000f6c         pop        rbp
0000000100000f6d         ret
```

main函数中调用t2(1,2,3,4)的反汇编结果，可以看到参数1，2，3，4分别传给了edi, esi, edx, ecx：

```
0000000100000f8e         mov        edi, 0x1
0000000100000f93         mov        esi, 0x2
0000000100000f98         mov        edx, 0x3
0000000100000f9d         mov        ecx, 0x4
0000000100000fa2         mov        dword [rbp+var_8], eax
0000000100000fa5         call       __Z2t2iiii                                  ; t2(int, int, int, int)
```

在t2中，又分别从这4个寄存器中取出值来放入内存。

```
0000000100000f54         mov        dword [rbp+var_4], edi
0000000100000f57         mov        dword [rbp+var_8], esi
0000000100000f5a         mov        dword [rbp+var_C], edx
0000000100000f5d         mov        dword [rbp+var_10], ecx
```

接着通过eax不断调用add命令累加。

如果多加几个参数呢：

```c
int add(int a, int b, int c, int d, int e, int f, int g, int h, int i) {
    return a + b + c + d + e + f + g + h + i;
}
```

反汇编后的代码：

```c
__text:0000000100003D00
__text:0000000100003D00 ; Attributes: bp-based frame
__text:0000000100003D00
__text:0000000100003D00 ; __int64 __fastcall add(int, int, int, int, int, int, int, int, int)
__text:0000000100003D00                 public __Z3addiiiiiiiii
__text:0000000100003D00 __Z3addiiiiiiiii proc near              ; CODE XREF: _main+DB↓p
__text:0000000100003D00
__text:0000000100003D00 var_1C          = dword ptr -1Ch
__text:0000000100003D00 var_18          = dword ptr -18h
__text:0000000100003D00 var_14          = dword ptr -14h
__text:0000000100003D00 var_10          = dword ptr -10h
__text:0000000100003D00 var_C           = dword ptr -0Ch
__text:0000000100003D00 var_8           = dword ptr -8
__text:0000000100003D00 var_4           = dword ptr -4
__text:0000000100003D00 arg_0           = dword ptr  10h
__text:0000000100003D00 arg_8           = dword ptr  18h
__text:0000000100003D00 arg_10          = dword ptr  20h
__text:0000000100003D00
__text:0000000100003D00                 push    rbp
__text:0000000100003D01                 mov     rbp, rsp
__text:0000000100003D04                 mov     eax, [rbp+20h]
__text:0000000100003D07                 mov     r10d, [rbp+18h]
__text:0000000100003D0B                 mov     r11d, [rbp+10h]
__text:0000000100003D0F                 mov     [rbp-4], edi
__text:0000000100003D12                 mov     [rbp-8], esi
__text:0000000100003D15                 mov     [rbp-0Ch], edx
__text:0000000100003D18                 mov     [rbp-10h], ecx
__text:0000000100003D1B                 mov     [rbp-14h], r8d
__text:0000000100003D1F                 mov     [rbp-18h], r9d
__text:0000000100003D23                 mov     ecx, [rbp-4]
__text:0000000100003D26                 add     ecx, [rbp-8]
__text:0000000100003D29                 add     ecx, [rbp-0Ch]
__text:0000000100003D2C                 add     ecx, [rbp-10h]
__text:0000000100003D2F                 add     ecx, [rbp-14h]
__text:0000000100003D32                 add     ecx, [rbp-18h]
__text:0000000100003D35                 add     ecx, [rbp+10h]
__text:0000000100003D38                 add     ecx, [rbp+18h]
__text:0000000100003D3B                 add     ecx, [rbp+20h]
__text:0000000100003D3E                 mov     [rbp-1Ch], eax
__text:0000000100003D41                 mov     eax, ecx
__text:0000000100003D43                 pop     rbp
__text:0000000100003D44                 retn
__text:0000000100003D44 __Z3addiiiiiiiii endp
```

调用的地方：

```c
int a = 0,b = 1,c = 2,d = 3,e = 4, f = 5, g = 6, h = 7, i = 8, j = 9;
add(a, b, c, d, e, f, g, h, i);
```

反汇编之后：

```c
__text:0000000100003E79                 mov     edi, [rbp-20h]  ; int
__text:0000000100003E7C                 mov     esi, [rbp-24h]  ; int
__text:0000000100003E7F                 mov     edx, [rbp-28h]  ; int
__text:0000000100003E82                 mov     ecx, [rbp-2Ch]  ; int
__text:0000000100003E85                 mov     r8d, [rbp-30h]  ; int
__text:0000000100003E89                 mov     r9d, [rbp-34h]  ; int
__text:0000000100003E8D                 mov     r11d, [rbp-38h]
__text:0000000100003E91                 mov     ebx, [rbp-3Ch]
__text:0000000100003E94                 mov     r14d, [rbp-40h]
__text:0000000100003E98                 mov     [rsp], r11d     ; int
__text:0000000100003E9C                 mov     [rsp+8], ebx    ; int
__text:0000000100003EA0                 mov     [rsp+10h], r14d ; int
__text:0000000100003EA5                 mov     [rbp-98h], eax
__text:0000000100003EAB                 call    __Z3addiiiiiiiii ; add(int,int,int,int,int,int,int,int,int)
```

可以看到几个整型的数据被放在了edi, esi, edx, ecx, r8d, r9d, 然后就是通过rsp（rbp）为基地址传递。

再看一下如果有多个指针参数的情况：

```c
int t2(int *c, int *d, int *e, int *f, int *g, int* h, int* i, int* j, int* k) {
    return *c + *d + *e + *f + *g + *h + *i + *j + *k;
}

int a = 0,b = 1,c = 2,d = 3,e = 4, f = 5, g = 6, h = 7, i = 8, j = 9;
t2(&a, &b, &c, &d, &e, &f, &h, &i, &j);
```

反汇编之后是：

```
__text:0000000100003E3C                 lea     rdi, [rbp+var_20] ; int *
__text:0000000100003E40                 lea     rsi, [rbp+var_24] ; int *
__text:0000000100003E44                 lea     rdx, [rbp+var_28] ; int *
__text:0000000100003E48                 lea     rcx, [rbp+var_2C] ; int *
__text:0000000100003E4C                 lea     r8, [rbp+var_30] ; int *
__text:0000000100003E50                 lea     r9, [rbp+var_34] ; int *
__text:0000000100003E54                 lea     r10, [rbp+var_3C]
__text:0000000100003E58                 mov     [rsp+0D0h+var_D0], r10 ; int *
__text:0000000100003E5C                 lea     r10, [rbp+var_40]
__text:0000000100003E60                 mov     [rsp+0D0h+var_C8], r10 ; int *
__text:0000000100003E65                 lea     r10, [rbp+var_44]
__text:0000000100003E69                 mov     [rsp+0D0h+var_C0], r10 ; int *
__text:0000000100003E6E                 mov     [rbp+var_94], eax
__text:0000000100003E74                 call    __Z2t2PiS_S_S_S_S_S_S_S_ ; t2(int *,int *,int *,int *,int *,int *,int *,int *,int *)
```

可以看到传递指针从左到右依次是：rdi, rsi, rdx, rcx, r8, r9, 然后就是基于rsp(rbp)的基地址传递。

其他可以观察的点：比如

- 函数的参数列表中类型不一样：int, char, float, double, 指针...
- 有自定义类型：struct,
- 函数的调用约定(call convertion)：
    - __cdecl   Pushes parameters on the stack, in reverse order (right to left)
    - __stdcall  Pushes parameters on the stack, in reverse order (right to left)
    - __thiscall Pushed on stack; this pointer stored in ECX
    - __fastcall Stored in registers, then pushed on stack
    - __clrcall Load parameters onto CLR expression stack in order (left to right).
    - __vectorcall Stored in registers, then pushed on stack in reverse order (right to left)

### 不同类型的参数之 -- 原生类型

自定义函数：

```
int t3(int a, char b, float c, int* p) {
    return a + (int)b + (int)c + *p;
}
```

反汇编结果：

```cpp
        ; ================ B E G I N N I N G   O F   P R O C E D U R E ================

        ; Variables:
        ;    var_4: -4
        ;    var_5: -5
        ;    var_C: -12
        ;    var_18: -24


                     __Z2t3icfPi:        // t3(int, char, float, int*)
0000000100000f10         push       rbp                                         ; CODE XREF=_main+90
0000000100000f11         mov        rbp, rsp
0000000100000f14         mov        dword [rbp+var_4], edi
0000000100000f17         mov        byte [rbp+var_5], sil
0000000100000f1b         movss      dword [rbp+var_C], xmm0
0000000100000f20         mov        qword [rbp+var_18], rdx
0000000100000f24         mov        eax, dword [rbp+var_4]
0000000100000f27         movsx      ecx, byte [rbp+var_5]
0000000100000f2b         add        eax, ecx
0000000100000f2d         cvttss2si  ecx, dword [rbp+var_C]
0000000100000f32         add        eax, ecx
0000000100000f34         mov        rdx, qword [rbp+var_18]
0000000100000f38         add        eax, dword [rdx]
0000000100000f3a         pop        rbp
0000000100000f3b         ret
```

可以看出，4个参数分别从edi，esi的低8位(sil)，xmm0，rdx中取出的。如果你有一点汇编基础知识就会知道：

- esi, edi, ebp, esp是32位的，对应的16位是si, di, bp, sp；而16位中的低8位分别是sil, dil, bpl, spl
- eax, ebx, ecx, edx是32位的，对应的16位ax, bx, cx, dx； 16位中的高8位分别是ah, bh, ch, dh， 低8位分别是al, bl, cl, dl。

调用t3的代码：

```
int v = 2;
t3(1, 'b', 3.14, &v);
```

反汇编结果：

```
0000000100000f7a         movss      xmm0, dword [0x100000fac]
0000000100000f82         mov        dword [rbp+var_8], 0x2
0000000100000f89         mov        edi, 0x1
0000000100000f8e         mov        esi, 0x62
0000000100000f93         lea        rdx, qword [rbp+var_8]
0000000100000f97         mov        dword [rbp+var_10], eax
0000000100000f9a         call       __Z2t3icfPi                                 ; t3(int, char, float, int*)
```

其中：

- 浮点数3.14

```
0000000100000f7a         movss      xmm0, dword [0x100000fac]
```

> MOVSS : `Move or Merge Scalar Single-Precision Floating-Point Value`，即专门保存单精度浮点数的。

[0x100000fac]就是3.14保存的位置，由于3.14是一个常量，专门放在了__const区，看代码。

```
        ; Section __const
        ; Range: [0x100000fac; 0x100000fb0[ (4 bytes)
        ; File offset : [4012; 4016[ (4 bytes)
        ;   S_REGULAR

0000000100000fac         dd         %1000000010010001111010111000011            ; DATA XREF=_main+58
```

Intel汇编语言中间接操作数是使用[]包裹起来表示的，比如[xxxx]，就表示保存在xxxx位置的数值。

[0x100000fac]位置的值，二进制表示为 [%1000000010010001111010111000011]，通过IEEE754表示方法展开：

符号位(1位)+指数部分(8位)+小数部分(23位)为：

0 10000000 10010001111010111000011

具体可参考: [如何手算十进制浮点数转二进制(IEEE754-32位)浮点数](https://www.jianshu.com/p/5760d471715a)中介绍的方法计算小数部分。或者参考 维基百科的[IEEE745](https://en.wikipedia.org/wiki/IEEE_754)。

地址[0x100000fac]中的值保存在了xmm0中。

- 指针&v

```
0000000100000f82         mov        dword [rbp+var_8], 0x2
```

是将0x2放入dword [rbp+var_8]中，后面这个临时变量的地址将被当作第四个参数传给t3：

```
0000000100000f93         lea        rdx, qword [rbp+var_8]
```

参数1保存在了edi，参数'b'保存在了sil，参数3.14保存在了xmm0中，参数&v保存在了rdx中。

#### 小结

看来gcc默认编译得到的代码，整型参数会依次从左到右按照edi, esi, edx, ecx的顺序传递，如果有浮点数参数，会依次保存到xmm0, xmm1...中;
如果是传递指针类型，那么64位系统上会使用rdi, rsi, rdx, rcx，r8, r9, 然后是基于rsp的地址传递.

测试机器使用的是Intel CPU的Mac系统。Intel CPU中，64bit模式下有16个通用寄存器，这16个寄存器可以兼容32it和64bit。

- 如果指定是32bit操作数，那么使用：EAX、EBX、ECX、EDX、EDI、ESI、EBP、ESP、R8 ～ R9；
- 如果指定64bit操作数，那么使用RAX、RBX、RCX、RDX、RRDI、ESI、RBP、RSP、R8 ～ R15。

所有这些寄存器都可以按照byte、word、dword、qword级别访问。

更多信息参考 [Intel® 64 and IA-32 Architectures Software Developer’s Manual Combined Volumes(1, 2A, 2B, 2C, 2D, 3A, 3B, 3C, 3D and 4)](https://software.intel.com/content/www/us/en/develop/download/intel-64-and-ia-32-architectures-sdm-combined-volumes-1-2a-2b-2c-2d-3a-3b-3c-3d-and-4.html)的 `3.4 BASIC PROGRAM EXECUTION REGISTERS`

以下内容引用自：[x64 Architecture](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/x64-architecture)

<table>
<colgroup>
<col width="25%" />
<col width="25%" />
<col width="25%" />
<col width="25%" />
</colgroup>
<thead>
<tr class="header">
<th align="left">64-bit register</th>
<th align="left">Lower 32 bits</th>
<th align="left">Lower 16 bits</th>
<th align="left">Lower 8 bits</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td align="left"><p><strong>rax</strong></p></td>
<td align="left"><p><strong>eax</strong></p></td>
<td align="left"><p><strong>ax</strong></p></td>
<td align="left"><p><strong>al</strong></p></td>
</tr>
<tr class="even">
<td align="left"><p><strong>rbx</strong></p></td>
<td align="left"><p><strong>ebx</strong></p></td>
<td align="left"><p><strong>bx</strong></p></td>
<td align="left"><p><strong>bl</strong></p></td>
</tr>
<tr class="odd">
<td align="left"><p><strong>rcx</strong></p></td>
<td align="left"><p><strong>ecx</strong></p></td>
<td align="left"><p><strong>cx</strong></p></td>
<td align="left"><p><strong>cl</strong></p></td>
</tr>
<tr class="even">
<td align="left"><p><strong>rdx</strong></p></td>
<td align="left"><p><strong>edx</strong></p></td>
<td align="left"><p><strong>dx</strong></p></td>
<td align="left"><p><strong>dl</strong></p></td>
</tr>
<tr class="odd">
<td align="left"><p><strong>rsi</strong></p></td>
<td align="left"><p><strong>esi</strong></p></td>
<td align="left"><p><strong>si</strong></p></td>
<td align="left"><p><strong>sil</strong></p></td>
</tr>
<tr class="even">
<td align="left"><p><strong>rdi</strong></p></td>
<td align="left"><p><strong>edi</strong></p></td>
<td align="left"><p><strong>di</strong></p></td>
<td align="left"><p><strong>dil</strong></p></td>
</tr>
<tr class="odd">
<td align="left"><p><strong>rbp</strong></p></td>
<td align="left"><p><strong>ebp</strong></p></td>
<td align="left"><p><strong>bp</strong></p></td>
<td align="left"><p><strong>bpl</strong></p></td>
</tr>
<tr class="even">
<td align="left"><p><strong>rsp</strong></p></td>
<td align="left"><p><strong>esp</strong></p></td>
<td align="left"><p><strong>sp</strong></p></td>
<td align="left"><p><strong>spl</strong></p></td>
</tr>
<tr class="odd">
<td align="left"><p><strong>r8</strong></p></td>
<td align="left"><p><strong>r8d</strong></p></td>
<td align="left"><p><strong>r8w</strong></p></td>
<td align="left"><p><strong>r8b</strong></p></td>
</tr>
<tr class="even">
<td align="left"><p><strong>r9</strong></p></td>
<td align="left"><p><strong>r9d</strong></p></td>
<td align="left"><p><strong>r9w</strong></p></td>
<td align="left"><p><strong>r9b</strong></p></td>
</tr>
<tr class="odd">
<td align="left"><p><strong>r10</strong></p></td>
<td align="left"><p><strong>r10d</strong></p></td>
<td align="left"><p><strong>r10w</strong></p></td>
<td align="left"><p><strong>r10b</strong></p></td>
</tr>
<tr class="even">
<td align="left"><p><strong>r11</strong></p></td>
<td align="left"><p><strong>r11d</strong></p></td>
<td align="left"><p><strong>r11w</strong></p></td>
<td align="left"><p><strong>r11b</strong></p></td>
</tr>
<tr class="odd">
<td align="left"><p><strong>r12</strong></p></td>
<td align="left"><p><strong>r12d</strong></p></td>
<td align="left"><p><strong>r12w</strong></p></td>
<td align="left"><p><strong>r12b</strong></p></td>
</tr>
<tr class="even">
<td align="left"><p><strong>r13</strong></p></td>
<td align="left"><p><strong>r13d</strong></p></td>
<td align="left"><p><strong>r13w</strong></p></td>
<td align="left"><p><strong>r13b</strong></p></td>
</tr>
<tr class="odd">
<td align="left"><p><strong>r14</strong></p></td>
<td align="left"><p><strong>r14d</strong></p></td>
<td align="left"><p><strong>r14w</strong></p></td>
<td align="left"><p><strong>r14b</strong></p></td>
</tr>
<tr class="even">
<td align="left"><p><strong>r15</strong></p></td>
<td align="left"><p><strong>r15d</strong></p></td>
<td align="left"><p><strong>r15w</strong></p></td>
<td align="left"><p><strong>r15b</strong></p></td>
</tr>
</tbody>
</table>

### 不同类型的参数之 -- struct类型

代码

```c
typedef struct _tagCustomStruct
{
    char a;
    int b;
    double d;
    void *p;
    _tagCustomStruct *next;
} CustomStruct;

int f4(int a, char b, float c, int *p, CustomStruct s)
{
    return a + (int)b + (int)c + *p + s.a + s.b + s.d;
}
```

main函数中源代码

```cpp
int main()
{
    int v = 2;
    CustomStruct *n = 0;
    CustomStruct s;
    s.a = 'a';
    s.b = 0xff;
    s.d = 3.14;
    s.p = &v;
    s.next = n;
    f4(1, 'b', 3.14, &v, s);

    return 0;
}
```

使用hopper反汇编之后为

```cpp
        ; ================ B E G I N N I N G   O F   P R O C E D U R E ================

        ; Variables:
        ;    var_4: -4
        ;    var_8: -8
        ;    var_10: -16
        ;    var_18: -24
        ;    var_20: -32
        ;    var_28: -40
        ;    var_2C: -44
        ;    var_30: -48
        ;    var_38: -56
        ;    var_40: -64
        ;    var_48: -72
        ;    var_50: -80
        ;    var_54: -84
        ;    var_68: -104
        ;    var_70: -112
        ;    var_78: -120
        ;    var_80: -128


                     _main:
0000000100000ee0         push       rbp                       ; rbp压栈
0000000100000ee1         mov        rbp, rsp                  ; rsp存入rbp，rbp开始为当前函数充当浮标
0000000100000ee4         sub        rsp, 0x80                 ; 调整rsp，申请到0x80个字节的临时变量空间
0000000100000eeb         movss      xmm0, dword [0x100000fa0] ; 保存float型数值3.14到xmm0寄存器中
0000000100000ef3         movsd      xmm1, qword [0x100000fa8] ; 保存double型数值3.14到xmm1寄存器中
0000000100000efb         mov        dword [rbp+var_4], 0x0    ; 
0000000100000f02         mov        dword [rbp+var_8], 0x2    ; int v = 2
0000000100000f09         mov        qword [rbp+var_10], 0x0   ; CustomStruct指针n = 0
0000000100000f11         mov        byte [rbp+var_30], 0x61   ; s.a = 'a'，注意单位是byte，一字节
0000000100000f15         mov        dword [rbp+var_2C], 0xff  ; s.b = 0xff 单位是DWORD，双字、4字节
0000000100000f1c         movsd      qword [rbp+var_28], xmm1  ; s.d = 3.14 单位是QWORD，四字、8字节
0000000100000f21         lea        rax, qword [rbp+var_8]    ; s.p = &v, 通过lea指令，将[rbp+var_8]的地址&v保存到rax中
0000000100000f25         mov        rcx, rax                  ; v的地址(&v)保存到rcx中
0000000100000f28         mov        qword [rbp+var_20], rcx   ; v的地址(&v)保存到一个临时变量中[rbp+var_20]
0000000100000f2c         mov        rcx, qword [rbp+var_10]   ; 指针n的值保存到rcx中
0000000100000f30         mov        qword [rbp+var_18], rcx   ; 指针n的值保存到临时变量[rbp+var_18]中
0000000100000f34         mov        rcx, qword [rbp+var_30]   ; s的地址(s.a的地址)保存到rcx中
0000000100000f38         mov        qword [rbp+var_50], rcx   ; rcx中保存的s的地址保存到临时变量[rbp+var_50]中
0000000100000f3c         mov        rcx, qword [rbp+var_28]   ; s.d（double型3.14）保存到rcx
0000000100000f40         mov        qword [rbp+var_48], rcx   ; rcx中的s.d保存到临时变量[rbp+var_48]中
0000000100000f44         mov        rcx, qword [rbp+var_20]   ; v的地址(&v)保存到rcx中
0000000100000f48         mov        qword [rbp+var_40], rcx   ; rcx中的值(&v)保存到[rbp+var_40]中
0000000100000f4c         mov        rcx, qword [rbp+var_18]   ; 指针n保存到rcx
0000000100000f50         mov        qword [rbp+var_38], rcx   ; rcx中指针n保存到临时变量[rbp+var_38]中
0000000100000f54         mov        edi, 0x1                  ; f4函数第一个参数1存入edi
0000000100000f59         mov        esi, 0x62                 ; f4函数第二个参数'b'存入esi
0000000100000f5e         mov        rdx, rax                  ; f4函数第四个参数&v存入rdx
0000000100000f61         lea        rax, qword [rbp+var_50]   ; 结构体s的地址存入rax。后面struct中的成员就可以用rax+offset来表示了
0000000100000f65         mov        rcx, qword [rax]          ; rax中的值(&s)存入rcx
0000000100000f68         mov        qword [rsp], rcx          ; rcx中的值(&s)存入rsp。后面struct的值存入了rsp，可以通过rsp+offset来表示了
0000000100000f6c         mov        rcx, qword [rax+8]        ; s.d存入rcx
0000000100000f70         mov        qword [rsp+8], rcx        ; rcx中的s.d存入[rsp+8]
0000000100000f75         mov        rcx, qword [rax+16]       ; s.p存入rcx
0000000100000f79         mov        qword [rsp+16], rcx       ; rcx中的s.p存入[rsp+16]
0000000100000f7e         mov        rax, qword [rax+24]       ; s.next存入rax
0000000100000f82         mov        qword [rsp+24], rax       ; rax中的值s.next存入rsp+24。这里需要注意struct s的值是通过rsp来传递的
0000000100000f87         call       __Z2f4icfPi16_tagCustomStruct               ; f4(int, char, float, int*, _tagCustomStruct)
0000000100000f8c         xor        esi, esi                  ; esi置0
0000000100000f8e         mov        dword [rbp+var_54], eax   ; 函数返回值（已存入eax）存入[rbp+var_54]
0000000100000f91         mov        eax, esi                  ; 设置eax为esi，即0，充当返回值0
0000000100000f93         add        rsp, 0x80                 ; rsp恢复
0000000100000f9a         pop        rbp                       ; rbp弹出，恢复
0000000100000f9b         ret                                  ; 函数返回
```

一起看一下源代码变化了之后，新的常量区(数据已切换为二进制形式显示)的代码:

```c
        ; Section __const
        ; Range: [0x100000fa0; 0x100000fb0[ (16 bytes)
        ; File offset : [4000; 4016[ (16 bytes)
        ;   S_REGULAR

0000000100000fa0         dq         %1000000010010001111010111000011            ; DATA XREF=_main+11
0000000100000fa8         dq         %100000000001001000111101011100001010001111010111000010100011111 ; DATA XREF=_main+19
```

> MOVSD: `Move or Merge Scalar Double-Precision Floating-Point Value`，即移动双精度浮点数的值。

*附加一条：下面是AT&T的语法：*

```c
asm`main:
    0x100000f10 <+0>:   pushq  %rbp
    0x100000f11 <+1>:   movq   %rsp, %rbp
    0x100000f14 <+4>:   subq   $0x60, %rsp
    0x100000f18 <+8>:   movss  0x88(%rip), %xmm0         ; xmm0 = mem[0],zero,zero,zero 
    0x100000f20 <+16>:  movsd  0x88(%rip), %xmm1         ; xmm1 = mem[0],zero 
    0x100000f28 <+24>:  movl   $0x0, -0x4(%rbp)
    0x100000f2f <+31>:  movl   $0x2, -0x8(%rbp)
    0x100000f36 <+38>:  movq   $0x0, -0x10(%rbp)
    0x100000f3e <+46>:  movb   $0x61, -0x30(%rbp)
    0x100000f42 <+50>:  movl   $0xff, -0x2c(%rbp)
    0x100000f49 <+57>:  movsd  %xmm1, -0x28(%rbp)
    0x100000f4e <+62>:  leaq   -0x8(%rbp), %rax
    0x100000f52 <+66>:  movq   %rax, %rcx
    0x100000f55 <+69>:  movq   %rcx, -0x20(%rbp)
    0x100000f59 <+73>:  movq   -0x10(%rbp), %rcx
    0x100000f5d <+77>:  movq   %rcx, -0x18(%rbp)
    0x100000f61 <+81>:  movl   $0x1, %edi
    0x100000f66 <+86>:  movl   $0x62, %esi
    0x100000f6b <+91>:  movq   %rax, %rdx
    0x100000f6e <+94>:  leaq   -0x30(%rbp), %rax
    0x100000f72 <+98>:  movq   (%rax), %rcx
    0x100000f75 <+101>: movq   %rcx, (%rsp)
    0x100000f79 <+105>: movq   0x8(%rax), %rcx
    0x100000f7d <+109>: movq   %rcx, 0x8(%rsp)
    0x100000f82 <+114>: movq   0x10(%rax), %rcx
    0x100000f86 <+118>: movq   %rcx, 0x10(%rsp)
    0x100000f8b <+123>: movq   0x18(%rax), %rax
    0x100000f8f <+127>: movq   %rax, 0x18(%rsp)
    0x100000f94 <+132>: callq  0x100000ec0               ; f4 
    0x100000f99 <+137>: xorl   %esi, %esi
    0x100000f9b <+139>: movl   %eax, -0x34(%rbp)
    0x100000f9e <+142>: movl   %esi, %eax
    0x100000fa0 <+144>: addq   $0x60, %rsp
    0x100000fa4 <+148>: popq   %rbp
    0x100000fa5 <+149>: retq   
```

*[AT&T汇编基本语法](https://chyyuu.gitbooks.io/ucore_os_docs/content/lab0/lab0_2_3_1_2_att_asm.html)：Xcode使用AT&T语法，gcc和msvc使用Intel汇编语法，二者有一些区别，不过区别不大。最主要的区别，我觉得还是操作数的顺序：AT&T是源操作数在左，目标操作数在右*

f4使用hopper反汇编之后为

```c
        ; ================ B E G I N N I N G   O F   P R O C E D U R E ================

        ; Variables:
        ;    arg_0: 16
        ;    var_4: -4
        ;    var_5: -5
        ;    var_C: -12
        ;    var_18: -24


                     __Z2f4icfPi16_tagCustomStruct:        // f4(int, char, float, int*, _tagCustomStruct)
0000000100000e90         push       rbp                     ; rbp压栈
0000000100000e91         mov        rbp, rsp                ; rsp存入rbp，rbp开始为函数充当浮标
0000000100000e94         lea        rax, qword [rbp+arg_0]  ; 取出s地址存到rax中
0000000100000e98         mov        dword [rbp+var_4], edi  ; edi中存的参数1存入[rbp+var_4]
0000000100000e9b         mov        byte [rbp+var_5], sil   ; esi中的低8bit值存入[rbp+var_5]
0000000100000e9f         movss      dword [rbp+var_C], xmm0 ; xmm0中的3.14f存入[rbp+var_C]
0000000100000ea4         mov        qword [rbp+var_18], rdx ; rdx中的&v存入[rbp+var_18]
0000000100000ea8         mov        ecx, dword [rbp+var_4]  ; 第一个参数a(1)存入ecx
0000000100000eab         movsx      edi, byte [rbp+var_5]   ; 第二个参数b('b')存入edi
0000000100000eaf         add        ecx, edi                ; a = a + b，并将a存入ecx
0000000100000eb1         cvttss2si  edi, dword [rbp+var_C]  ; float 3.14f转为DWORD类型，放入edi(即为3)
0000000100000eb6         add        ecx, edi                ; edi中的值累加到ecx
0000000100000eb8         mov        rdx, qword [rbp+var_18] ; rbp+var_18中的值(参数指针p的地址)存入rdx
0000000100000ebc         add        ecx, dword [rdx]        ; rdx的值所指的地址中的值（*p）累加入ecx
0000000100000ebe         movsx      edi, byte [rax]         ; 取出[rax]中的低字节(s.a)存入edi
0000000100000ec1         add        ecx, edi                ; edi中的值累加入ecx
0000000100000ec3         add        ecx, dword [rax+4]      ; [rax+4]位置的值s.b累加入ecx
0000000100000ec6         cvtsi2sd   xmm0, ecx               ; 将ecx中到目前为止累加的值转为双精度浮点型
0000000100000eca         addsd      xmm0, qword [rax+8]     ; s.d中的值累加入xmm0
0000000100000ecf         cvttsd2si  eax, xmm0               ; 将xmm0中的双精度浮点型转为双字整型存入eax，最终的累加值保存在了eax中
0000000100000ed3         pop        rbp                     ; 恢复rbp的值
0000000100000ed4         ret                                ; 函数返回
```

> CVTSS2SI的意思是：`CVTSS2SI—Convert Scalar Single-Precision Floating-Point Value to Doubleword Integer` 就是将float转为双字整型。

> CVTSI2SD的意思是：`CVTSI2SD—Convert Doubleword Integer to Scalar Double-Precision Floating-Point Value`，就是强转双字整型为双精度浮点型数值

> CVTSD2SI的意思是：`CVTSD2SI—Convert Scalar Double-Precision Floating-Point Value to Doubleword Integer`，就是双精度浮点型值转为双字整型

> MOVSX - `MOVSX/MOVSXD—Move with Sign-Extension` 就是移动带符号数值

> CVTTSS2SI的意思是：`Convert with Truncation Scalar Single-Precision Floating-Point Value to Integer`，就是将源操作数单精度浮点型转为双字整型并放入目标操作数

#### 小结

从上面的分析中可以看出，整型参数的传递使用来edi, esi, rdx, rcx， 浮点型使用来xmm0, xmm1...，而struct类型是通过rsp传递的。那要是有多个struct参数呢？

### 不同类型参数之 -- 多个struct类型参数

代码

```c

int t1(int a, int b) {
    int r = a + b;
    return r;
}

int t2(int *c, int *d, int *e, int *f, int *g, int* h, int* i, int* j, int* k) {
    return *c + *d + *e + *f + *g + *h + *i + *j + *k;
}

int add(int a, int b, int c, int d, int e, int f, int g, int h, int i) {
    return a + b + c + d + e + f + g + h + i;
}

int t3(int a, char b, float c, int* p) {
    return a + (int)b + (int)c + *p;
}

typedef struct _tagCustomStruct
{
    char a;
    int b;
    double d;
    void *p;
    _tagCustomStruct *next;
} CustomStruct;

int f4(int a, char b, float c, int *p, CustomStruct s1, CustomStruct s2)
{
    return a + (int)b + (int)c + *p + s1.a + s1.b + s2.d;
}

int main() {
    t1(2,3);
    int a = 0,b = 1,c = 2,d = 3,e = 4, f = 5, g = 6, h = 7, i = 8, j = 9;
    t2(&a, &b, &c, &d, &e, &f, &h, &i, &j);
    add(a, b, c, d, e, f, g, h, i);
    int v = 2;
    t3(1,'a', 3.14f, &v);
    CustomStruct *n = 0;
    CustomStruct s1, s2;
    s1.a = 'a';
    s1.b = 0xff;
    s1.d = 3.14;
    s1.p = &v;
    s1.next = n;
    s2.a = 'b';
    s2.b = 0xee;
    s2.d = 4.0;
    s2.p = &v;
    s2.next = &s1;
    f4(1, 'b', 3.14, &v, s1, s2);

    return 0;
}
```

使用IDA64反汇编之后：

```c
__text:0000000100003CE0 ; =============== S U B R O U T I N E =======================================
__text:0000000100003CE0
__text:0000000100003CE0 ; Attributes: bp-based frame
__text:0000000100003CE0
__text:0000000100003CE0 ; f4(int, char, float, int *, _tagCustomStruct, _tagCustomStruct)
__text:0000000100003CE0                 public __Z2f4icfPi16_tagCustomStructS0_
__text:0000000100003CE0 __Z2f4icfPi16_tagCustomStructS0_ proc near
__text:0000000100003CE0                                         ; CODE XREF: _main+234↓p
__text:0000000100003CE0
__text:0000000100003CE0 var_18          = qword ptr -18h
__text:0000000100003CE0 var_C           = dword ptr -0Ch
__text:0000000100003CE0 var_5           = byte ptr -5
__text:0000000100003CE0 var_4           = dword ptr -4
__text:0000000100003CE0 arg_0           = byte ptr  10h
__text:0000000100003CE0 arg_20          = byte ptr  30h
__text:0000000100003CE0
__text:0000000100003CE0                 push    rbp
__text:0000000100003CE1                 mov     rbp, rsp
__text:0000000100003CE4                 lea     rax, [rbp+arg_20]   ; 取出s2的地址，可见是通过rbp/rsp取到的struct参数地址
__text:0000000100003CE8                 lea     rcx, [rbp+arg_0]    ; 取出s1的地址，可见是通过rbp/rsp取得struct参数地址
__text:0000000100003CEC                 mov     [rbp+var_4], edi    ; 从edi中取出第一个参数
__text:0000000100003CEF                 mov     [rbp+var_5], sil    ; 从sil中取出第二个参数
__text:0000000100003CF3                 movss   [rbp+var_C], xmm0   ; 从xmm0中取出第三个参数
__text:0000000100003CF8                 mov     [rbp+var_18], rdx   ; 从rdx中取出第四个参数
__text:0000000100003CFC                 mov     edi, [rbp+var_4]    ; 
__text:0000000100003CFF                 movsx   r8d, [rbp+var_5]    ; 
__text:0000000100003D04                 add     edi, r8d            ; a + (int)b，结果存入edi
__text:0000000100003D07                 cvttss2si r8d, [rbp+var_C]  ; (int)c float转为int类型，并存入r8d
__text:0000000100003D0D                 add     edi, r8d            ; 累加入edi(原存放a的寄存器)
__text:0000000100003D10                 mov     rdx, [rbp+var_18]   ; 第四个参数的地址存入rdx
__text:0000000100003D14                 add     edi, [rdx]          ; 解引用第四个参数(*p)累加入edi
__text:0000000100003D16                 movsx   r8d, byte ptr [rcx] ; 以byte级别取出s1的前8bit，也就是s1.a，放入r8d
__text:0000000100003D1A                 add     edi, r8d            ; s1.a累加入edi
__text:0000000100003D1D                 add     edi, [rcx+4]        ; [rcx+4]就是s1.b，累加入edi
__text:0000000100003D20                 cvtsi2sd xmm0, edi          ; edi中的整型转为浮点型并放入xmm0
__text:0000000100003D24                 addsd   xmm0, qword ptr [rax+8]; 累加浮点数s2.d到xmm0
__text:0000000100003D29                 cvttsd2si eax, xmm0         ; xmm0中的结果转为双字整型并存入eax，作为返回值
__text:0000000100003D2D                 pop     rbp
__text:0000000100003D2E                 retn
```

从上面的分析过程可知，struct类型的参数，无论一个还是2个都是通过栈（rbp/rsp）传递的。

调用的地方：

```c
CustomStruct *n = 0;
CustomStruct s1, s2;
s1.a = 'a';
s1.b = 0xff;
s1.d = 3.14;
s1.p = &v;
s1.next = n;
s2.a = 'b';
s2.b = 0xee;
s2.d = 4.0;
s2.p = &v;
s2.next = &s1;
f4(1, 'b', 3.14, &v, s1, s2);
```

使用IDA64反汇编之后（截取的是部分反汇编代码，这里的寻址表示方法就使用数值而不是IDA默认的tag的方式了）

```c
__text:0000000100003E38                 movss   xmm0, cs:dword_100003F88
__text:0000000100003E40                 movsd   xmm1, cs:qword_100003F90    ; 4.0
__text:0000000100003E48                 movsd   xmm2, cs:qword_100003F98    ; 3.14
__text:0000000100003E50                 mov     qword ptr [rbp-50h], 0      ; n = 0;
__text:0000000100003E58                 mov     byte ptr [rbp-70h], 61h     ; 常数载入内存：'a'  (s1.a)
__text:0000000100003E5C                 mov     dword ptr [rbp-6Ch], 0FFh   ; 常数载入内存：0xff (s1.b)
__text:0000000100003E63                 movsd   qword ptr [rbp-68h], xmm2   ; 常数载入内存：3.14 (s1.d)
__text:0000000100003E68                 lea     rdx, [rbp-48h]              ; v的地址存入rdx
__text:0000000100003E6C                 mov     r10, rdx                    ; v存入r10
__text:0000000100003E6F                 mov     [rbp-60h], r10              ; s1.p = &v; [rbp-60h]对应s1.p
__text:0000000100003E73                 mov     r10, [rbp-50h]              ; n存入r10
__text:0000000100003E77                 mov     [rbp-58h], r10              ; s1.next = n
__text:0000000100003E7B                 mov     byte ptr [rbp-90h], 62h     ; 常数载入内存：'b'  (s2.a)
__text:0000000100003E82                 mov     dword ptr [rbp-8Ch], 0EEh   ; 常数载入内存：0xee (s2.b)
__text:0000000100003E8C                 movsd   qword ptr [rbp-88h], xmm1   ; 常数载入内存：4.0  (s2.d)
__text:0000000100003E94                 mov     r10, rdx                    ; 目前存了v的地址
__text:0000000100003E97                 mov     [rbp-80h], r10              ; &v
__text:0000000100003E9B                 lea     r10, [rbp-70h]              ; 'a'所在地址存入r10，也就是s1
__text:0000000100003E9F                 mov     [rbp-78h], r10              ; s1的地址存入[rbp-78h]
__text:0000000100003EA3                 mov     r10, [rbp-70h]              ; s1的前8byte，从这一步开始准备将s1放入基于rbp的栈上
__text:0000000100003EA7                 mov     [rbp-0B0h], r10             ; s1的前8byte存入[rbp-0B0h]
__text:0000000100003EAE                 mov     r10, [rbp-68h]              ; 3.14存入r10
__text:0000000100003EB2                 mov     [rbp-0A8h], r10             ; 3.14存入[rbp-0A8h]
__text:0000000100003EB9                 mov     r10, [rbp-60h]              ; &v([rbp-60h])存入r10
__text:0000000100003EBD                 mov     [rbp-0A0h], r10             ; &v存入[rbp-0A0h]
__text:0000000100003EC4                 mov     r10, [rbp-58h]              ; s1.next存入r10
__text:0000000100003EC8                 mov     [rbp-98h], r10              ; s1.next[rbp-98h]，s1在栈上的数据初始化完毕。
__text:0000000100003ECF                 mov     r10, [rbp-90h]              ; 'b'存入r10， 从这一步开始，将s2放入基于rbp的栈上。
__text:0000000100003ED6                 mov     [rbp-0D0h], r10             ; 'b'存入[rbp-0D0h]
__text:0000000100003EDD                 mov     r10, [rbp-88h]              ; 4.0存入r10
__text:0000000100003EE4                 mov     [rbp-0C8h], r10             ; 4.0存入[rbp-0C8h]
__text:0000000100003EEB                 mov     r10, [rbp-80h]              ; &v存入r10
__text:0000000100003EEF                 mov     [rbp-0C0h], r10             ; (s2.p)&v存入[rbp-0C0h]
__text:0000000100003EF6                 mov     r10, [rbp-78h]              ; s1的地址存入r10
__text:0000000100003EFA                 mov     [rbp-0B8h], r10             ; s1的地址存入[rbp-0B8h]，这个地址是s2.next，完成了s2.next=&s1，s2在栈上的数据初始化完毕。
__text:0000000100003F01                 mov     edi, 1                      ; 准备f4的第一个参数，放入edi
__text:0000000100003F06                 mov     esi, 62h                    ; 准备f4的第二个参数，放入esi
__text:0000000100003F0B                 lea     r10, [rbp-0B0h]             ; s1的地址存入r10
__text:0000000100003F12                 mov     r15, [r10]                  ; s1的地址存入r15
__text:0000000100003F15                 mov     [rsp], r15                  ; s1的地址存入rsp
__text:0000000100003F19                 mov     r15, [r10+8]                ; s1.d(3.14)存入r15，开始基于r10拷贝s1的各个字段到基于rsp的地址上。
__text:0000000100003F1D                 mov     [rsp+8], r15                ; s1.d(3.14)存入[rsp+8]
__text:0000000100003F22                 mov     r15, [r10+10h]              ; &v的值存入r15
__text:0000000100003F26                 mov     [rsp+10h], r15              ; &v存入[rsp+10h]
__text:0000000100003F2B                 mov     r10, [r10+18h]              ; s1.next存入r10
__text:0000000100003F2F                 mov     [rsp+18h], r10              ; s1.next存入[rsp+18h]
__text:0000000100003F34                 lea     r10, [rbp-0D0h]             ; s2的地址存入r10
__text:0000000100003F3B                 mov     r15, [r10]                  ; s2的地址存入r15
__text:0000000100003F3E                 mov     [rsp+20h], r15              ; s2的地址存入[rsp+20h]
__text:0000000100003F43                 mov     r15, [r10+8]                ; 基于r10，开始拷贝s2的各个字段，到基于rsp的地址上，这次是s2.d(4.0)
__text:0000000100003F47                 mov     [rsp+28h], r15              ; s2.d（4.0）放入[rsp+28h]
__text:0000000100003F4C                 mov     r15, [r10+10h]              ; s2.p放入r15
__text:0000000100003F50                 mov     [rsp+30h], r15              ; s2.p放入[rsp+30h]
__text:0000000100003F55                 mov     r10, [r10+18h]              ; s2.next放入r10
__text:0000000100003F59                 mov     [rsp+38h], r10              ; s2.next放入[rsp+38h]，到此s1, s2都放在了基于rsp的地址上。
__text:0000000100003F5E                 mov     [rbp-0E0h], eax             ; 上一个函数调用的返回值，存入[rbp-0E0h]
__text:0000000100003F64                 call    __Z2f4icfPi16_tagCustomStructS0_ ; f4(int,char,float,int *,_tagCustomStruct,_tagCustomStruct)
```
> 提示：要分析上述汇编代码，最好在纸上画一下栈上的数据布局，这样看代码才直观。

> 从上面的过程中可以看出，准备struct参数的时候，首先是将即时数（常数）直接载入栈上，然后再存入通用寄存器；接着是将其他的struct参数，一个一个地存入栈上内存。最后通过rsp/rbp传递参数。

其中引用到的纯数据段:

```c
__const:0000000100003F88 ; ===========================================================================
__const:0000000100003F88
__const:0000000100003F88 ; Segment type: Pure data
__const:0000000100003F88 ; Segment alignment 'qword' can not be represented in assembly
__const:0000000100003F88 __const         segment para public 'DATA' use64
__const:0000000100003F88                 assume cs:__const
__const:0000000100003F88                 ;org 100003F88h
__const:0000000100003F88 ; float dword_100003F88
__const:0000000100003F88 dword_100003F88 dd 4048F5C3h            ; DATA XREF: _main+E0↑r
__const:0000000100003F88                                         ; _main+108↑r
__const:0000000100003F8C                 align 10h
__const:0000000100003F90 qword_100003F90 dq 4010000000000000h    ; DATA XREF: _main+110↑r
__const:0000000100003F98 qword_100003F98 dq 40091EB851EB851Fh    ; DATA XREF: _main+118↑r
__const:0000000100003F98 __const         ends
__const:0000000100003F98
```

如果换作二进制表示：

```c
__const:0000000100003F88 ; ===========================================================================
__const:0000000100003F88
__const:0000000100003F88 ; Segment type: Pure data
__const:0000000100003F88 ; Segment alignment 'qword' can not be represented in assembly
__const:0000000100003F88 __const         segment para public 'DATA' use64
__const:0000000100003F88                 assume cs:__const
__const:0000000100003F88                 ;org 100003F88h
__const:0000000100003F88 ; float dword_100003F88
__const:0000000100003F88 dword_100003F88 dd 1000000010010001111010111000011b
__const:0000000100003F88                                         ; DATA XREF: _main+E0↑r
__const:0000000100003F88                                         ; _main+108↑r
__const:0000000100003F8C                 align 10h
__const:0000000100003F90 qword_100003F90 dq 100000000010000000000000000000000000000000000000000000000000000b
__const:0000000100003F90                                         ; DATA XREF: _main+110↑r
__const:0000000100003F98 qword_100003F98 dq 100000000001001000111101011100001010001111010111000010100011111b
__const:0000000100003F98                                         ; DATA XREF: _main+118↑r
__const:0000000100003F98 __const         ends
__const:0000000100003F98
```

不过还是使用Hopper打开，可以有更多的表示方法，直接使用float的方式展示：

```c
        ; Section __const
        ; Range: [0x100003f88; 0x100003fa0[ (24 bytes)
        ; File offset : [16264; 16288[ (24 bytes)
        ;   S_REGULAR

0000000100003f88         dq         0.000000                                    ; DATA XREF=_main+224, _main+264
0000000100003f90         dq         4.000000                                    ; DATA XREF=_main+272
0000000100003f98         dq         3.140000                                    ; DATA XREF=_main+280
```

在IDA中可以创建自定义的struct方便查看反汇编代码：

```c
00000000 CustomStruct    struc ; (sizeof=0x20, mappedto_22)
00000000 a               dw ?                    ; char
00000002 dummy           dw ?
00000004 b               dd ?
00000008 d               dq ?
00000010 p               dq ?
00000018 next            dq ?
00000020 CustomStruct    ends
```

具体方法参考：[IDA: Structures Tutorial](https://www.hex-rays.com/products/ida/support/tutorials/structs/) or [IDA Help: Define a new structure](https://www.hex-rays.com/products/ida/support/idadoc/491.shtml)。

比如IDA中自定义struct之后可以将上面的反汇编代码写作(具体方法是对着需要转化类型的正偏移值按下按钮T，在弹出的对话框中选择对应的类型)：

```c
__text:0000000100003F01                 mov     edi, 1
__text:0000000100003F06                 mov     esi, 62h
__text:0000000100003F0B                 lea     r10, [rbp-0B0h]
__text:0000000100003F12                 mov     r15, [r10]
__text:0000000100003F15                 mov     qword ptr [rsp+CustomStruct.a], r15
__text:0000000100003F19                 mov     r15, [r10+8]
__text:0000000100003F1D                 mov     [rsp+CustomStruct.d], r15
__text:0000000100003F22                 mov     r15, [r10+10h]
__text:0000000100003F26                 mov     [rsp+CustomStruct.p], r15
__text:0000000100003F2B                 mov     r10, [r10+18h]
__text:0000000100003F2F                 mov     [rsp+CustomStruct.next], r10
__text:0000000100003F34                 lea     r10, [rbp-0D0h]
__text:0000000100003F3B                 mov     r15, [r10]
__text:0000000100003F3E                 mov     [rsp+(size CustomStruct)], r15
__text:0000000100003F43                 mov     r15, [r10+8]
__text:0000000100003F47                 mov     [rsp+(CustomStruct.d+20h)], r15
__text:0000000100003F4C                 mov     r15, [r10+10h]
__text:0000000100003F50                 mov     [rsp+(CustomStruct.p+20h)], r15
__text:0000000100003F55                 mov     r10, [r10+18h]
__text:0000000100003F59                 mov     [rsp+(CustomStruct.next+20h)], r10
__text:0000000100003F5E                 mov     [rbp-0E0h], eax
__text:0000000100003F64                 call    __Z2f4icfPi16_tagCustomStructS0_ ; f4(int,char,float,int *,_tagCustomStruct,_tagCustomStruct)
```

IDA不能对基于负的偏移地址做struct引用，所以这里只能对基于rsp的正的偏移地址做符号标记。

#### 小结

对于struct类型的参数，在传递的时候使用的是基于rbp/rsp的方式。其中，准备参数环节是使用基于rsp的方式放置到栈上的，在取出参数的环节是通过rbp取出的。而在被调用函数开头段落中 `push rbp` `mov rbp rsp` 环节会将rsp赋与rbp。从而做好了rsp与rbp的关联。

# MSVC同样一个简单的例子

```c
int t1(int a, int b) {
    int r = a + b;
    return r;
}

int main() {
    t1(2, 3);
    return 0;
}
```

> 尝试了一下使用IDA反汇编Win32的代码，结果反汇编出来的效果很差，连main函数都找不到，不知道为什么！！！所以，后面暂时使用VS自己的反汇编代码。

VS反汇编x86的debug版本之后：

```
int t1(int a, int b) {
008F16F0  push        ebp  
008F16F1  mov         ebp,esp  
008F16F3  sub         esp,0CCh  
008F16F9  push        ebx  
008F16FA  push        esi  
008F16FB  push        edi  
008F16FC  lea         edi,[ebp-0CCh]  
008F1702  mov         ecx,33h  
008F1707  mov         eax,0CCCCCCCCh  
008F170C  rep stos    dword ptr es:[edi]  
008F170E  mov         ecx,offset _C8498636_simple-asm@cpp (08FC000h)  
008F1713  call        @__CheckForDebuggerJustMyCode@4 (08F1208h)  
    int r = a + b;
008F1718  mov         eax,dword ptr [a]  
008F171B  add         eax,dword ptr [b]  
008F171E  mov         dword ptr [r],eax  
    return r;
008F1721  mov         eax,dword ptr [r]  
}
008F1724  pop         edi  
008F1725  pop         esi  
008F1726  pop         ebx  
008F1727  add         esp,0CCh  
008F172D  cmp         ebp,esp  
008F172F  call        __RTC_CheckEsp (08F1212h)  
008F1734  mov         esp,ebp  
008F1736  pop         ebp  
008F1737  ret  
```

VS编译出来的代码中有一个比较特殊的函数 `@__CheckForDebuggerJustMyCode@4`， 这个是MSVC编译器为了支持本地代码调试加的特性，具体参考 [/JMC (Just My Code debugging)](https://docs.microsoft.com/en-us/cpp/build/reference/jmc?view=msvc-160)，这里为了排除这些因素对反汇编代码的影响，先设置关闭：` Configuration Properties > C/C++ > General property page 设置为关闭`，之后可以得到下面的代码：

```cpp
int t1(int a, int b) {
001A16E0  push        ebp  
001A16E1  mov         ebp,esp  
001A16E3  sub         esp,0CCh  
001A16E9  push        ebx  
001A16EA  push        esi  
001A16EB  push        edi  
001A16EC  lea         edi,[ebp-0CCh]  
001A16F2  mov         ecx,33h  
001A16F7  mov         eax,0CCCCCCCCh  
001A16FC  rep stos    dword ptr es:[edi]  
    int r = a + b;
001A16FE  mov         eax,dword ptr [a]  
001A1701  add         eax,dword ptr [b]  
001A1704  mov         dword ptr [r],eax  
    return r;
001A1707  mov         eax,dword ptr [r]  
}
001A170A  pop         edi  
001A170B  pop         esi  
001A170C  pop         ebx  
001A170D  mov         esp,ebp  
001A170F  pop         ebp  
001A1710  ret  
```

main函数

```
int main() {
008F1760  push        ebp  
008F1761  mov         ebp,esp  
008F1763  sub         esp,0C0h  
008F1769  push        ebx  
008F176A  push        esi  
008F176B  push        edi  
008F176C  lea         edi,[ebp-0C0h]  
008F1772  mov         ecx,30h  
008F1777  mov         eax,0CCCCCCCCh  
008F177C  rep stos    dword ptr es:[edi]  
008F177E  mov         ecx,offset _C8498636_simple-asm@cpp (08FC000h)  

int main() {
008F1783  call        @__CheckForDebuggerJustMyCode@4 (08F1208h)  
    t1(2, 3);
008F1788  push        3  
008F178A  push        2  
008F178C  call        t1 (08F10AFh)  
008F1791  add         esp,8  
    return 0;
008F1794  xor         eax,eax  
}
008F1796  pop         edi  
008F1797  pop         esi  
008F1798  pop         ebx  
008F1799  add         esp,0C0h  
008F179F  cmp         ebp,esp  
008F17A1  call        __RTC_CheckEsp (08F1212h)  
008F17A6  mov         esp,ebp  
008F17A8  pop         ebp  
008F17A9  ret  
```

关闭/JMC之后反汇编代码为：

```cpp
int main() {
00DE1720  push        ebp  
00DE1721  mov         ebp,esp  
00DE1723  sub         esp,0C0h  
00DE1729  push        ebx  
00DE172A  push        esi  
00DE172B  push        edi  
00DE172C  lea         edi,[ebp-0C0h]  
00DE1732  mov         ecx,30h  
00DE1737  mov         eax,0CCCCCCCCh  
00DE173C  rep stos    dword ptr es:[edi]  
    t1(2, 3);
00DE173E  push        3  
00DE1740  push        2  
00DE1742  call        t1 (0DE10AAh)  
00DE1747  add         esp,8  
    return 0;
00DE174A  xor         eax,eax  
}
00DE174C  pop         edi  
00DE174D  pop         esi  
00DE174E  pop         ebx  
}
00DE174F  add         esp,0C0h  
00DE1755  cmp         ebp,esp  
00DE1757  call        __RTC_CheckEsp (0DE120Dh)  
00DE175C  mov         esp,ebp  
00DE175E  pop         ebp  
00DE175F  ret  
```

# Clang编译同一个代码

## 最简单的例子

还是从最简单的代码看起。

```cpp
int t1(int a, int b) {
    int r = a + b;
    return r;
}

int t2(int c, int d, int e, int f) {
    return c + d + e + f;
}

int t3(int a, char b, float c, int* p) {
    return a + (int)b + (int)c + *p;
}

typedef struct _tagCustomStruct
{
    char   a;
    int    b;
    double d;
    void  *p;
    _tagCustomStruct *next;
} CustomStruct;

int f4(int a, char b, float c, int *p, CustomStruct s)
{
    return a + (int)b + (int)c + *p + s.a + s.b + s.d;
}

int main() {
    t1(2,3);
    t2(4,5,6,3.14);
    int v = 2;
    t3(1,'a', 3.14f, &v);
    CustomStruct *n = 0;
    CustomStruct s;
    s.a = 'a';
    s.b = 0xff;
    s.d = 3.14;
    s.p = &v;
    s.next = n;
    f4(1, 'b', 3.14, &v, s);

    return 0;
}
```

t1函数使用IDA64反汇编之后，注意t1的调用约定是__fastcall，源码中没有指定，可以认为clang会默认使用这种方式：

```cpp
__text:0000000100003E20 ; =============== S U B R O U T I N E =======================================
__text:0000000100003E20
__text:0000000100003E20 ; Attributes: bp-based frame
__text:0000000100003E20
__text:0000000100003E20 ; __int64 __fastcall t1(int, int)
__text:0000000100003E20                 public __Z2t1ii
__text:0000000100003E20 __Z2t1ii        proc near
__text:0000000100003E20
__text:0000000100003E20 var_C           = dword ptr -0Ch
__text:0000000100003E20 var_8           = dword ptr -8
__text:0000000100003E20 var_4           = dword ptr -4
__text:0000000100003E20
__text:0000000100003E20                 push    rbp
__text:0000000100003E21                 mov     rbp, rsp
__text:0000000100003E24                 mov     [rbp+var_4], edi    ; 从左到右第一个参数存入edi
__text:0000000100003E27                 mov     [rbp+var_8], esi    ; 从左到右第二个参数存入esi
__text:0000000100003E2A                 mov     eax, [rbp+var_4]
__text:0000000100003E2D                 add     eax, [rbp+var_8]
__text:0000000100003E30                 mov     [rbp+var_C], eax    ; 累加结果存入eax，然后存入本地变量[rbp+var_C]
__text:0000000100003E33                 mov     eax, [rbp+var_C]    ; debug版本编译出来的中二代码，再把[rbp+var_C]中的值存回eax，最终函数的返回值（累加值）保存在eax中
__text:0000000100003E36                 pop     rbp
__text:0000000100003E37                 retn
__text:0000000100003E37 __Z2t1ii        endp
```

t2函数使用IDA64反汇编之后，t2的调用约定是__fastcall。

```cpp
__text:0000000100003E38                 align 20h
__text:0000000100003E40
__text:0000000100003E40 ; =============== S U B R O U T I N E =======================================
__text:0000000100003E40
__text:0000000100003E40 ; Attributes: bp-based frame
__text:0000000100003E40
__text:0000000100003E40 ; __int64 __fastcall t2(int, int, int, int)
__text:0000000100003E40                 public __Z2t2iiii
__text:0000000100003E40 __Z2t2iiii      proc near
__text:0000000100003E40
__text:0000000100003E40 var_10          = dword ptr -10h
__text:0000000100003E40 var_C           = dword ptr -0Ch
__text:0000000100003E40 var_8           = dword ptr -8
__text:0000000100003E40 var_4           = dword ptr -4
__text:0000000100003E40
__text:0000000100003E40                 push    rbp
__text:0000000100003E41                 mov     rbp, rsp
__text:0000000100003E44                 mov     [rbp+var_4], edi ; 从左到右取出第一个参数edi，存入rbp+var_4
__text:0000000100003E47                 mov     [rbp+var_8], esi ; 从左到右取出第一个参数esi，存入[rbp+var_8]
__text:0000000100003E4A                 mov     [rbp+var_C], edx ; 从左到右取出第三个参数edx，存入[rbp+var_C]
__text:0000000100003E4D                 mov     [rbp+var_10], ecx; 从左到右取出第四个参数ecx，存入[rbp+var_10]
__text:0000000100003E50                 mov     eax, [rbp+var_4] ; 接下来四条指令，依次累加临时变量中的值到eax中。
__text:0000000100003E53                 add     eax, [rbp+var_8]
__text:0000000100003E56                 add     eax, [rbp+var_C]
__text:0000000100003E59                 add     eax, [rbp+var_10]
__text:0000000100003E5C                 pop     rbp
__text:0000000100003E5D                 retn
__text:0000000100003E5D __Z2t2iiii      endp
```

t3函数使用IDA64反汇编之后，t3的调用约定是__fastcall：

```cpp
__text:0000000100003E5E                 align 20h
__text:0000000100003E60
__text:0000000100003E60 ; =============== S U B R O U T I N E =======================================
__text:0000000100003E60
__text:0000000100003E60 ; Attributes: bp-based frame
__text:0000000100003E60
__text:0000000100003E60 ; __int64 __fastcall t3(int, char, float, int *)
__text:0000000100003E60                 public __Z2t3icfPi
__text:0000000100003E60 __Z2t3icfPi     proc near
__text:0000000100003E60
__text:0000000100003E60 var_18          = qword ptr -18h
__text:0000000100003E60 var_C           = dword ptr -0Ch
__text:0000000100003E60 var_5           = byte ptr -5
__text:0000000100003E60 var_4           = dword ptr -4
__text:0000000100003E60
__text:0000000100003E60                 push    rbp
__text:0000000100003E61                 mov     rbp, rsp
__text:0000000100003E64                 mov     [rbp+var_4], edi  ; edi保存了第一个参数
__text:0000000100003E67                 mov     [rbp+var_5], sil  ; esi的低8bit保存了第二个参数
__text:0000000100003E6B                 movss   [rbp+var_C], xmm0 ; xmm0中保存了第三个参数float型数值
__text:0000000100003E70                 mov     [rbp+var_18], rdx ; rdx中保存了第四个参数,int*类型
__text:0000000100003E74                 mov     eax, [rbp+var_4]  ; 
__text:0000000100003E77                 movsx   ecx, [rbp+var_5]
__text:0000000100003E7B                 add     eax, ecx
__text:0000000100003E7D                 cvttss2si ecx, [rbp+var_C]; [rbp+var_C]中保存的float数值转化为双字整型
__text:0000000100003E82                 add     eax, ecx
__text:0000000100003E84                 mov     rdx, [rbp+var_18]
__text:0000000100003E88                 add     eax, [rdx]
__text:0000000100003E8A                 pop     rbp
__text:0000000100003E8B                 retn
__text:0000000100003E8B __Z2t3icfPi     endp
```

f4函数使用IDA64反汇编之后，注意f4的调用约定没有明确指定：

```cpp
__text:0000000100003E8C                 align 10h
__text:0000000100003E90
__text:0000000100003E90 ; =============== S U B R O U T I N E =======================================
__text:0000000100003E90
__text:0000000100003E90 ; Attributes: bp-based frame
__text:0000000100003E90
__text:0000000100003E90 ; f4(int, char, float, int *, _tagCustomStruct)
__text:0000000100003E90                 public __Z2f4icfPi16_tagCustomStruct
__text:0000000100003E90 __Z2f4icfPi16_tagCustomStruct proc near ; CODE XREF: _main+A7↓p
__text:0000000100003E90
__text:0000000100003E90 var_18          = qword ptr -18h
__text:0000000100003E90 var_C           = dword ptr -0Ch
__text:0000000100003E90 var_5           = byte ptr -5
__text:0000000100003E90 var_4           = dword ptr -4
__text:0000000100003E90 arg_0           = byte ptr  10h
__text:0000000100003E90
__text:0000000100003E90                 push    rbp
__text:0000000100003E91                 mov     rbp, rsp
__text:0000000100003E94                 lea     rax, [rbp+arg_0]    ; [rbp+arg_0]保存的指针存入rax，也就是第五个参数的地址
__text:0000000100003E98                 mov     [rbp+var_4], edi    ; 第一个参数edi
__text:0000000100003E9B                 mov     [rbp+var_5], sil    ; 第二个参数esi的低8bit
__text:0000000100003E9F                 movss   [rbp+var_C], xmm0   ; 第三个参数xmm0
__text:0000000100003EA4                 mov     [rbp+var_18], rdx   ; 第四个参数
__text:0000000100003EA8                 mov     ecx, [rbp+var_4]    ; 第一个参数存入ecx
__text:0000000100003EAB                 movsx   edi, [rbp+var_5]    ; 第二个参数存入edi
__text:0000000100003EAF                 add     ecx, edi            ; 前两个参数累加入ecx
__text:0000000100003EB1                 cvttss2si edi, [rbp+var_C]  ; 第三个参数转为整型存入edi
__text:0000000100003EB6                 add     ecx, edi            ; 第三个参数累加入ecx
__text:0000000100003EB8                 mov     rdx, [rbp+var_18]   ; 第四个参数取出
__text:0000000100003EBC                 add     ecx, [rdx]          ; 第四个参数累加入ecx
__text:0000000100003EBE                 movsx   edi, byte ptr [rax] ; 
__text:0000000100003EC1                 add     ecx, edi
__text:0000000100003EC3                 add     ecx, [rax+4]
__text:0000000100003EC6                 cvtsi2sd xmm0, ecx
__text:0000000100003ECA                 addsd   xmm0, qword ptr [rax+8]
__text:0000000100003ECF                 cvttsd2si eax, xmm0
__text:0000000100003ED3                 pop     rbp
__text:0000000100003ED4                 retn
__text:0000000100003ED4 __Z2f4icfPi16_tagCustomStruct endp
```

也是基于rbp的方式取出传入的struct类型参数。

main函数使用IDA64反汇编之后，注意main函数的调用约定是__cdecl：

```cpp
__text:0000000100003E95                 align 20h
__text:0000000100003EA0
__text:0000000100003EA0 ; =============== S U B R O U T I N E =======================================
__text:0000000100003EA0
__text:0000000100003EA0 ; Attributes: bp-based frame
__text:0000000100003EA0
__text:0000000100003EA0 ; int __cdecl main(int argc, const char **argv, const char **envp)
__text:0000000100003EA0                 public _main
__text:0000000100003EA0 _main           proc near
__text:0000000100003EA0
__text:0000000100003EA0 var_80          = qword ptr -80h
__text:0000000100003EA0 var_78          = qword ptr -78h
__text:0000000100003EA0 var_70          = qword ptr -70h
__text:0000000100003EA0 var_68          = qword ptr -68h
__text:0000000100003EA0 var_60          = dword ptr -60h
__text:0000000100003EA0 var_5C          = dword ptr -5Ch
__text:0000000100003EA0 var_58          = dword ptr -58h
__text:0000000100003EA0 var_54          = dword ptr -54h
__text:0000000100003EA0 var_50          = qword ptr -50h
__text:0000000100003EA0 var_48          = qword ptr -48h
__text:0000000100003EA0 var_40          = qword ptr -40h
__text:0000000100003EA0 var_38          = qword ptr -38h
__text:0000000100003EA0 var_30          = qword ptr -30h
__text:0000000100003EA0 var_28          = qword ptr -28h
__text:0000000100003EA0 var_20          = qword ptr -20h
__text:0000000100003EA0 var_18          = qword ptr -18h
__text:0000000100003EA0 var_10          = qword ptr -10h
__text:0000000100003EA0 var_8           = dword ptr -8
__text:0000000100003EA0 var_4           = dword ptr -4
__text:0000000100003EA0
__text:0000000100003EA0                 push    rbp
__text:0000000100003EA1                 mov     rbp, rsp
__text:0000000100003EA4                 sub     rsp, 80h
__text:0000000100003EAB                 mov     [rbp+var_4], 0
__text:0000000100003EB2                 mov     edi, 2          ; 从左到右第一个参数存入edi
__text:0000000100003EB7                 mov     esi, 3          ; 第二个参数存入esi
__text:0000000100003EBC                 call    __Z2t1ii        ; t1(int,int)
__text:0000000100003EC1                 mov     edi, 4          ; 第一个参数
__text:0000000100003EC6                 mov     esi, 5          ; 第二个参数
__text:0000000100003ECB                 mov     edx, 6          ; 第三个参数
__text:0000000100003ED0                 mov     ecx, 3          ; 第四个参数3.14
__text:0000000100003ED5                 mov     [rbp+var_54], eax   ; t1函数返回值暂存入[rbp+var_54]
__text:0000000100003ED8                 call    __Z2t2iiii      ; t2(int,int,int,int)
__text:0000000100003EDD                 movss   xmm0, cs:dword_100003FA8 ; 从左到右第三个擦数存入xmm0，
__text:0000000100003EE5                 mov     [rbp+var_8], 2  ; 临时变量v
__text:0000000100003EEC                 mov     edi, 1          ; 从左到右第一个参数存入edi
__text:0000000100003EF1                 mov     esi, 61h        ; 从左到右第二个参数存入esi
__text:0000000100003EF6                 lea     rdx, [rbp+var_8] ; 操作从左到右第四个参数，v的地址存入rdx
__text:0000000100003EFA                 mov     [rbp+var_58], eax ; t2函数的返回值eax存入临时变量[rbp+var_58]
__text:0000000100003EFD                 call    __Z2t3icfPi     ; t3(int,char,float,int *)
__text:0000000100003F02                 movss   xmm0, cs:dword_100003FA8    ; 这条指令开始准备f4的参数。这一条是float数3.14f存入xmm0
__text:0000000100003F0A                 movsd   xmm1, cs:qword_100003FB0    ; double数3.14存入xmm1
__text:0000000100003F12                 mov     [rbp+var_10], 0     ; [rbp+var_10]指向的8byte设置为0，应该是n=0;
__text:0000000100003F1A                 mov     byte ptr [rbp+var_30], 61h  ; s.a = '1'
__text:0000000100003F1E                 mov     dword ptr [rbp+var_30+4], 0FFh; s.b = 0xff
__text:0000000100003F25                 movsd   [rbp+var_28], xmm1  ; s.d = 3.14 double类型的3.14
__text:0000000100003F2A                 lea     rdx, [rbp+var_8]    ; s.p = &v,前面可知[rbp+var_8]中是v
__text:0000000100003F2E                 mov     r8, rdx             ; &v存入r8
__text:0000000100003F31                 mov     [rbp+var_20], r8    ; &v存入临时变量[rbp+var_20]
__text:0000000100003F35                 mov     r8, [rbp+var_10]    ; r8存入指针n
__text:0000000100003F39                 mov     [rbp+var_18], r8    ; 地址[rbp+var_18]存入n(0)
__text:0000000100003F3D                 mov     r8, [rbp+var_30]    ; s.a存入r8
__text:0000000100003F41                 mov     [rbp+var_50], r8    ; s.a存入[rbp_var_50]
__text:0000000100003F45                 mov     r8, [rbp+var_28]    ; s.d存入r8
__text:0000000100003F49                 mov     [rbp+var_48], r8    ; s.d存入[rbp+var_48]
__text:0000000100003F4D                 mov     r8, [rbp+var_20]    ; s.p存入r8
__text:0000000100003F51                 mov     [rbp+var_40], r8    ; s.p存入[rbp+var_40]
__text:0000000100003F55                 mov     r8, [rbp+var_18]    ; r8存入n
__text:0000000100003F59                 mov     [rbp+var_38], r8    ; n存入[rbp+var_38]；到这里结构体s的初始化工作完成
__text:0000000100003F5D                 mov     edi, 1              ; f1从左到右的第一个参数1，存入edi
__text:0000000100003F62                 mov     esi, 62h            ; f1从左到右的第二个参数'a'存入esi
__text:0000000100003F67                 lea     r8, [rbp+var_50]    ; s的地址存入r8
__text:0000000100003F6B                 mov     r9, [r8]            ; s的地址前8B存入r9
__text:0000000100003F6E                 mov     [rsp], r9           ; s的数据存入[rsp](也就是s.a, s.b)
__text:0000000100003F72                 mov     r9, [r8+8]          ; s.d存入r9(3.14)
__text:0000000100003F76                 mov     [rsp+80h+var_78], r9; s.d存入临时变量 [rsp+80h+var_78]
__text:0000000100003F7B                 mov     r9, [r8+10h]        ; s.p存入r9
__text:0000000100003F7F                 mov     [rsp+80h+var_70], r9; s.p存入临时变量 [rsp+80h+var_70]
__text:0000000100003F84                 mov     r8, [r8+18h]        ; s.next存入r8
__text:0000000100003F88                 mov     [rsp+80h+var_68], r8; s.next存入[rsp+80h+var_68]
__text:0000000100003F8D                 mov     [rbp+var_5C], eax   ; t3调用之后的返回值eax保存在临时变量[rbp+var_5C]
__text:0000000100003F90                 call    __Z2f4icfPi16_tagCustomStruct ; f4(int,char,float,int *,_tagCustomStruct) ; 调用f4
__text:0000000100003F95                 xor     ecx, ecx
__text:0000000100003F97                 mov     [rbp+var_60], eax
__text:0000000100003F9A                 mov     eax, ecx
__text:0000000100003F9C                 add     rsp, 80h
__text:0000000100003FA3                 pop     rbp
__text:0000000100003FA4                 retn
__text:0000000100003FA4 _main           endp
```

基于rsp准备好传入的struct参数。原生类型使用了与gcc一样的方式传递。

> 顺便分析一下函数t3编译后的命名 `__Z2t3icfPi` ：__Z2是__fastcall的开头，t3为函数名，i表示第一个参数为整型，c表示char，f表示float，Pi表示整型指针
> `__Z2f4icfPi16_tagCustomStruct` ：

如果要使用Xcode跟踪汇编代码的话，记住使用 `p $eax` 查看寄存器eax的值；使用 `x $eax` 查看寄存器eax中保存的指针引用的值。或者参考[GDB to LLDB command map](https://lldb.llvm.org/use/map.html)中的register read命令。比如：`register read --format binary r9`，其中format参数可以选择：

```c
"default"
'B' or "boolean"
'b' or "binary"
'y' or "bytes"
'Y' or "bytes with ASCII"
'c' or "character"
'C' or "printable character"
'F' or "complex float"
's' or "c-string"
'd' or "decimal"
'E' or "enumeration"
'x' or "hex"
'X' or "uppercase hex"
'f' or "float"
'o' or "octal"
'O' or "OSType"
'U' or "unicode16"
"unicode32"
'u' or "unsigned decimal"
'p' or "pointer"
"char[]"
"int8_t[]"
"uint8_t[]"
"int16_t[]"
"uint16_t[]"
"int32_t[]"
"uint32_t[]"
"int64_t[]"
"uint64_t[]"
"float16[]"
"float32[]"
"float64[]"
"uint128_t[]"
'I' or "complex integer"
'a' or "character array"
'A' or "address"
"hex float"
'i' or "instruction"
'v' or "void"
'u' or "unicode8"
```

Pure data段数据：

```cpp
__const:0000000100003FA8 ; ===========================================================================
__const:0000000100003FA8
__const:0000000100003FA8 ; Segment type: Pure data
__const:0000000100003FA8 ; Segment alignment 'qword' can not be represented in assembly
__const:0000000100003FA8 __const         segment para public 'DATA' use64
__const:0000000100003FA8                 assume cs:__const
__const:0000000100003FA8                 ;org 100003FA8h
__const:0000000100003FA8 ; float dword_100003FA8
__const:0000000100003FA8 dword_100003FA8 dd 4048F5C3h            ; DATA XREF: _main+3D↑r
__const:0000000100003FA8                                         ; _main+62↑r
__const:0000000100003FAC                 align 10h
__const:0000000100003FB0 qword_100003FB0 dq 40091EB851EB851Fh    ; DATA XREF: _main+6A↑r
__const:0000000100003FB0 __const         ends
```

## 不同类型参数之 -- 多个struct

clang编译得到的上述代码，与gcc的完全一致。可以参考gcc中的内容。

## 小结

可以发现，clang在处理上述参数传递的时候，使用了与gcc一样的方式。

# 不同编译器的差异

现在主流的CPU架构有多种，比如区分32/64位的桌面端CPU平台，还有ARM平台。编译器针对x86/x64以及ARM有不同的处理方法，下面介绍的调用约定中需要考虑这些。

> 参考
> - 维基百科 [x86 calling conventions](https://en.wikipedia.org/wiki/X86_calling_conventions)，[Application binary interface](https://en.wikipedia.org/wiki/Application_binary_interface)
> - [clang - calling conventions](https://clang.llvm.org/docs/AttributeReference.html#calling-conventions)
> - [gcc - x86-Function-Attributes](https://gcc.gnu.org/onlinedocs/gcc/x86-Function-Attributes.html#x86-Function-Attributes)
> - [MSVC - Calling Conventions](https://docs.microsoft.com/en-us/cpp/cpp/calling-conventions?view=msvc-160), [x64 calling convention](https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-160)。不同的调用约定会影响最后链接的名称，参考：[Decorated Names](https://docs.microsoft.com/en-us/cpp/build/reference/decorated-names?view=msvc-160)
> - [What is the calling convention that clang uses?](https://stackoverflow.com/questions/56718751/what-is-the-calling-convention-that-clang-uses)

## 函数约定带来的影响

调用约定（call convention）描述了：

1. 原生类型、变长类型参数的各个部分如何分配内存
2. 参数传递方式（堆栈、寄存器，二者混用）
3. 被调用者必须为调用者保留哪些寄存器
4. 函数调用如何分配堆栈，记忆调用完毕之后如何还原堆栈

> 应用程序二进制接口(ABI - Application binary interface)包括了：调用约定，类型表示，名称修改。

而不同的编译器会有不止一种平台的实现，比如Intel CPU，PowerPC，ARM等等。为此专门有函数属性支持设置具体的调用约定类型。

## MSVC支持的调用约定

### __cdecl

- **使用范围**：C/C\++程序默认的调用约定。ARM和x64程序中编译器会忽略这个约定，因为ARM和x64程序会尽量使用寄存器，然后才是stack。
- **参数传递顺序**：参数从右向左压栈
- **特点**：可用于变参传递
- **谁来清空stack**：caller(调用函数)负责弹出stack上的内存；所以可用于传递变参。

/Gd编译选项强制函数使用__cdecl调用约定。

```cpp
struct CMyClass {
   void __cdecl mymethod();
};

等价于
void CMyClass::mymethod() { return; }

等价于
void __cdecl CMyClass::mymethod() { return; }
```

ref: [__cdecl](https://docs.microsoft.com/en-us/cpp/cpp/cdecl?view=msvc-160)

### __clrcall

**使用范围**：只能被托管代码调用

ref：[__clrcall](https://docs.microsoft.com/en-us/cpp/cpp/clrcall?view=msvc-160)

### __stdcall

- **使用范围**：用于Win32 API函数
- **参数传递顺序**：参数从右向左压栈，默认传值，除非传递指针或者引用
- **特点**：可用于变参传递
- **谁来清空stack**：callee(被调函数)负责弹出stack上的内存。

/Gz编译选项指定所有未明确指定调用约定的函数为__stdcall。

### __fastcall

- **使用范围**：仅可用于x86架构，微软编译器专用
- **参数传递顺序**：尽可能使用寄存器；从左向右参数列表中前两个DWORD类型或者更小类型的参数传递给ECX和EDX。所有其他参数按照从右向左的顺序压栈。
- **特点**：@作为编译后名称前缀；对于x64架构，前四个参数尽可能传给寄存器，其他参数传给栈；对于ARM架构，最多可以有4个整型或者8个浮点型传给寄存器，其他参数传给栈。
- **谁来清空stack**：callee(被调函数)负责弹出stack上的内存

ref: [__fastcall](https://docs.microsoft.com/en-us/cpp/cpp/fastcall?view=msvc-160)

### __thiscall  

- **使用范围**：微软编译器特有的编译选项，非可变参数的成员函数默认使用这种类型。
- **参数传递顺序**：参数从右向左压栈
- **特点**：this指针通过ECX传递，而不是通过栈。
- **谁来清空stack**：callee(被调用函数)负责弹出stack上的内存；

ref: [__thiscall](https://docs.microsoft.com/en-us/cpp/cpp/thiscall?view=msvc-160)

### __vectorcall

- **使用范围**：微软编译器特有的编译选项；仅可用于包括SIMD 2（SSE2）及以上的x86/x64处理器的native code。
- **参数传递顺序**：参数从右向左压栈
- **特点**：尽可能使用寄存器传递参数，比__fastcall和默认的x64调用约定使用更多的寄存器。
- **谁来清空stack**：callee(被调用函数)负责弹出stack上的内存；

/Gv编译选项可以设置模块中除了成员函数之外的函数编译为__vectorcall。

这种调用约定支持三种类型的参数传入寄存器：（integer type）整型，（vector type）向量型，homogeneous vector aggregate（HVA）型。

- 整型：要满足两个条件。其一，大小满足处理器本地寄存器的大小，比如x86对应4Byte，x64对应8Byte；其二，转换类型来回转换不改变值的bit位，比如x86上的int，x64上的long long类型。整型具体包括指针，引用，以及x86上4Byte的struct和union，或者x64上8Byte的struct和union。x64架构中，大的struct和union类型由调用函数申请内存并通过指针或者引用类型传递；而x86系统上通过栈来传递值。
- 向量型：浮点类型（float，double）或者SIMD向量类型（__m128，__m256）
- HVA型：是最多4个相同向量类型的组合。比如：

```
typedef struct {
   __m256 x;
   __m256 y;
   __m256 z;
} hva3;    // 3 element HVA type on __m256
```

成员函数可以声明为__vectorcall类型。this指针会被当做第一个整型传递给寄存器。

ARM机器上这种声明可能被忽略，也可能被遵循。

ref：[__vectorcall](https://docs.microsoft.com/en-us/cpp/cpp/vectorcall?view=msvc-160)

### 汇总

|关键字	| 清理栈 | 参数传递顺序 |
|---|---|---|
|__cdecl|	调用者	|Pushes parameters on the stack, in reverse order (right to left)
|__clrcall|	n/a	|Load parameters onto CLR expression stack in order (left to right).
|__stdcall|	Callee|	Pushes parameters on the stack, in reverse order (right to left)
|__fastcall|	Callee|	Stored in registers, then pushed on stack
|__thiscall|	Callee|	Pushed on stack; this pointer stored in ECX
|__vectorcall|	Callee|	Stored in registers, then pushed on stack in reverse order (right to left)

## clang编译器支持的调用约定

TODO：
【这里】
https://clang.llvm.org/docs/AttributeReference.html#calling-conventions

- aarch64_vector_pcs

fastcall

ms_abi

pcs

preserve_all

preserve_most

regcall

regparm

stdcall

thiscall

vectorcall

# 参考

- [Practical C++ Decompilation](http://www.hexblog.com/wp-content/uploads/2011/08/Recon-2011-Skochinsky.pdf)
- [普林斯顿大学的一个教程](https://www.cs.princeton.edu/courses/archive/spr16/cos217/lectures/)