[toc]

# Mac上最简单的一个例子

首先Mac上的代码，基本可以认为是x64的代码：

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

```
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

```
        ; ================ B E G I N N I N G   O F   P R O C E D U R E ================

        ; Variables:
        ;    var_4: -4
        ;    var_8: -8


                     _main:
0000000100000f80         push       rbp
0000000100000f81         mov        rbp, rsp
0000000100000f84         sub        rsp, 0x10
0000000100000f88         mov        dword [rbp+var_4], 0x0
0000000100000f8f         mov        edi, 0x2
0000000100000f94         mov        esi, 0x3
0000000100000f99         call       __Z2t1ii                                    ; t1(int, int)
0000000100000f9e         xor        ecx, ecx
0000000100000fa0         mov        dword [rbp+var_8], eax
0000000100000fa3         mov        eax, ecx
0000000100000fa5         add        rsp, 0x10
0000000100000fa9         pop        rbp
0000000100000faa         ret
```

观察几个点：

## 函数调用时候的标准化步骤

main函数状态栈的变化：

```cpp
push rbp
mov  rbp rsp
```

main函数调用状态出栈

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
0000000100000f54         mov        dword [rbp+var_4], edi
0000000100000f57         mov        dword [rbp+var_8], esi
0000000100000f5a         mov        dword [rbp+var_C], edx
0000000100000f5d         mov        dword [rbp+var_10], ecx
0000000100000f60         mov        eax, dword [rbp+var_4]
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

在t2中，又分别中这4个寄存器中取出值来放入内存。

```
0000000100000f54         mov        dword [rbp+var_4], edi
0000000100000f57         mov        dword [rbp+var_8], esi
0000000100000f5a         mov        dword [rbp+var_C], edx
0000000100000f5d         mov        dword [rbp+var_10], ecx
```

接着通过eax不断调用add命令累加。

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

可以看出，4个参数分辨从edi，esi的低8位(sil)，xmm0，rdx中取出的。如果你有一点汇编基础知识就会知道：

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

*movss的含义是：`MOVSS—Move or Merge Scalar Single-Precision Floating-Point Value`，即专门保存单精度浮点数的。*

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

看来整型参数会依次从左到右按照edi, esi, rdx, rcx的顺序传递，如果有浮点数参数，会依次保存到xmm0, xmm1...中

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

反汇编之后为

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

*movsd的意思是 `MOVSD — Move or Merge Scalar Double-Precision Floating-Point Value`，即移动双精度浮点数的值。*

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

f4反汇编之后为

```cpp
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
0000000100000eb1         cvttss2si  edi, dword [rbp+var_C]  ; float 3.14f转为DWORD 3.14，放入edi(即为3)
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

CVTSS2SI的意思是：`CVTSS2SI—Convert Scalar Single-Precision Floating-Point Value to Doubleword Integer` 就是将float转为双字整型。

CVTSI2SD的意思是：`CVTSI2SD—Convert Doubleword Integer to Scalar Double-Precision Floating-Point Value`，就是强转双字整型为双精度浮点型数值

CVTSD2SI的意思是：`CVTSD2SI—Convert Scalar Double-Precision Floating-Point Value to Doubleword Integer`，就是双精度浮点型值转为双字整型

MOVSX - `MOVSX/MOVSXD—Move with Sign-Extension` 就是移动值带符号数值

#### 小结

从上面的分析中可以看出，整型参数的传递使用来edi, esi, rdx, rcx， 浮点型使用来xmm0, xmm1...，而struct类型是通过rsp传递的。那要是有多个struct参数呢？

# Windows上同样一个简单的例子

```
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

### 不同编译器的差异





### 函数约定带来的影响

调用约定（call convention）描述了：

1. 原生类型、变长类型参数的各个部分如何分配内存
2. 参数传递方式（堆栈、寄存器，二者混用）
3. 被调用者必须为调用者保留哪些寄存器
4. 函数调用如何分配堆栈，记忆调用完毕之后如何还原堆栈

> 应用程序二进制接口(ABI - Application binary interface)包括了：调用约定，类型表示，名称修改。

而不同的编译器会有不止一种平台的实现，比如Intel CPU，PowerPC，ARM等等。为此专门有函数属性支持设置具体的调用约定类型。

#### 微软编译器支持的调用约定

##### __cdecl

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

##### __clrcall

**使用范围**：只能被托管代码调用

ref：[__clrcall](https://docs.microsoft.com/en-us/cpp/cpp/clrcall?view=msvc-160)

##### __stdcall

- **使用范围**：用于Win32 API函数
- **参数传递顺序**：参数从右向左压栈，默认传值，除非传递指针或者引用
- **特点**：可用于变参传递
- **谁来清空stack**：callee(被调函数)负责弹出stack上的内存。

/Gz编译选项指定所有未明确指定调用约定的函数为__stdcall。

##### __fastcall

- **使用范围**：仅可用于x86架构，微软编译器专用
- **参数传递顺序**：尽可能使用寄存器；从左向右参数列表中前两个DWORD类型或者更小类型的参数传递给ECX和EDX。所有其他参数按照从右向左的顺序压栈。
- **特点**：@作为编译后名称前缀；对于x64架构，前四个参数尽可能传给寄存器，其他参数传给栈；对于ARM架构，最多可以有4个整型或者8个浮点型传给寄存器，其他参数传给栈。
- **谁来清空stack**：callee(被调函数)负责弹出stack上的内存

ref: [__fastcall](https://docs.microsoft.com/en-us/cpp/cpp/fastcall?view=msvc-160)

##### __thiscall  

- **使用范围**：微软编译器特有的编译选项，非可变参数的成员函数默认使用这种类型。
- **参数传递顺序**：参数从右向左压栈
- **特点**：this指针通过ECX传递，而不是通过栈。
- **谁来清空stack**：callee(被调用函数)负责弹出stack上的内存；

ref: [__thiscall](https://docs.microsoft.com/en-us/cpp/cpp/thiscall?view=msvc-160)

##### __vectorcall

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

##### 汇总

|关键字	| 清理栈 | 参数传递顺序 |
|---|---|---|
|__cdecl|	调用者	|Pushes parameters on the stack, in reverse order (right to left)
|__clrcall|	n/a	|Load parameters onto CLR expression stack in order (left to right).
|__stdcall|	Callee|	Pushes parameters on the stack, in reverse order (right to left)
|__fastcall|	Callee|	Stored in registers, then pushed on stack
|__thiscall|	Callee|	Pushed on stack; this pointer stored in ECX
|__vectorcall|	Callee|	Stored in registers, then pushed on stack in reverse order (right to left)

#### clang编译器支持的调用约定

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

### 参考

> - 维基百科 [x86 calling conventions](https://en.wikipedia.org/wiki/X86_calling_conventions)，[Application binary interface](https://en.wikipedia.org/wiki/Application_binary_interface)
> - [clang - calling conventions](https://clang.llvm.org/docs/AttributeReference.html#calling-conventions)
> - [gcc - x86-Function-Attributes](https://gcc.gnu.org/onlinedocs/gcc/x86-Function-Attributes.html#x86-Function-Attributes)
> - [MSVC - Calling Conventions](https://docs.microsoft.com/en-us/cpp/cpp/calling-conventions?view=msvc-160), [x64 calling convention](https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-160)。不同的调用约定会影响最后链接的名称，参考：[Decorated Names](https://docs.microsoft.com/en-us/cpp/build/reference/decorated-names?view=msvc-160)
> - [What is the calling convention that clang uses?](https://stackoverflow.com/questions/56718751/what-is-the-calling-convention-that-clang-uses)


# 参考

- [Practical C++ Decompilation](http://www.hexblog.com/wp-content/uploads/2011/08/Recon-2011-Skochinsky.pdf)
- [普林斯顿大学的一个教程](https://www.cs.princeton.edu/courses/archive/spr16/cos217/lectures/)