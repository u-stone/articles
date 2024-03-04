[toc]
# Windows 下 MSVC 的 cpp 对象内存布局

打开VS新建一个控制台程序，然后写入以下代码：

```
#include <iostream>

class Base {
public:
    Base() {}
    int a = 0;
    int b = 0;
};

class Base2 {
    virtual void f1() = 0;
};

class Derived : public Base {
public:
    Derived() :Base() {}

    char c = 0;
    int d = 0;
};

class Derived2 : public Base, public Base2 {
public:
    virtual void f1() {}
    double f = 0.0;
};

class Derived3 : public Base, public Base2 {
    virtual void f1() {}
    virtual void f2() {}
};

class BaseV : public Base2 {
    virtual void f1() {

    }
};

class BaseV2 : public Base2 {
    virtual void f1() {

    }
};

class Derived4 : virtual BaseV, virtual BaseV2 {
    virtual void f3() {

    }
};

int main()
{
    //Derived3 d;
    Derived4 d4;
    return 0;
}
```

之后打开配置属性->C/C++->命令行，然后添加编译指令：/d1reportSingleClassLayoutDerived, 然后开始编译。就会在输出窗口看到下面的信息：

```
  class Derived	size(16):
  	+---
   0	| +--- (base class Base)
   0	| | a
   4	| | b
  	| +---
   8	| c
    	| <alignment member> (size=3)
  12	| d
  	+---

  class Derived2	size(24):
  	+---
   0	| +--- (base class Base2)
   0	| | {vfptr}
  	| +---
   4	| +--- (base class Base)
   4	| | a
   8	| | b
  	| +---
    	| <alignment member> (size=4)
  16	| f
  	+---

  Derived2::$vftable@:
  	| &Derived2_meta
  	|  0
   0	| &Derived2::f1

  Derived2::f1 this adjustor: 0

  class Derived3	size(12):
  	+---
   0	| +--- (base class Base2)
   0	| | {vfptr}
  	| +---
   4	| +--- (base class Base)
   4	| | a
   8	| | b
  	| +---
  	+---

  Derived3::$vftable@:
  	| &Derived3_meta
  	|  0
   0	| &Derived3::f1
   1	| &Derived3::f2

  Derived3::f1 this adjustor: 0
  Derived3::f2 this adjustor: 0

  class Derived4	size(16):
  	+---
   0	| {vfptr}
   4	| {vbptr}
  	+---
  	+--- (virtual base BaseV)
   8	| +--- (base class Base2)
   8	| | {vfptr}
  	| +---
  	+---
  	+--- (virtual base BaseV2)
  12	| +--- (base class Base2)
  12	| | {vfptr}
  	| +---
  	+---

  Derived4::$vftable@Derived4@:
  	| &Derived4_meta
  	|  0
   0	| &Derived4::f3

  Derived4::$vbtable@:
   0	| -4
   1	| 4 (Derived4d(Derived4+4)BaseV)
   2	| 8 (Derived4d(Derived4+4)BaseV2)

  Derived4::$vftable@BaseV@:
  	| -8
   0	| &BaseV::f1

  Derived4::$vftable@BaseV2@:
  	| -12
   0	| &BaseV2::f1

  Derived4::f3 this adjustor: 0
  vbi:	   class  offset o.vbptr  o.vbte fVtorDisp
             BaseV       8       4       4 0
            BaseV2      12       4       8 0
```

可以看到C++对象的布局。可以发现：
1. 类中的虚函数数量不影响类对象的大小
2. 但是，如果类中只要有一个虚函数，就会有一个虚表指针，也就是会增大4byte（32位程序）。
3. 虚表放在了其他地方，仅需要虚指针可以找到即可，所以不占用对象大小，参考`Derived2::$vftable@:和Derived3::$vftable@:`。
4. 虚基类和非虚基类同时被继承的话，内存上先做虚基类的布局。参考Derived2。

# C++ 虚继承

*本篇源码部分来自clang，部分来自《C++程序设计语言 4th》的21章。*

本篇是在学/复习虚继承的相关概念，发现这个一般用不到的概念还是有很多东西没有理解了的。

## 什么时候用到虚继承

有的时候，在类设计上，将数据、接口放在了基类中，就可能会产生了菱形继承结构。比如：

```c
class Storable {
public:
    Storable(const string&);
    virtual void read() = 0;
    virtual void write() = 0;
    virtual Storable();
    
protected:
    string file_name;
    Storable(Storable&) = delete;
    Storable operator=(Storable&) = delete;
};

class Transmitter : public Storable {
public:
    void write() override;
    // ...
};

class Receiver : public virtual Storable {
public:
    void read() override;
    // ...
};

class Radio : public Transmitter, public Receiver {
public:
    void write() override;
    // ...
};

```

除了数据的共享之外，还有接口的重用。

那么，菱形继承有什么问题呢？答：数据和接口都会有两份。数据会占用内存，接口方面最终的派生类会搞不清楚到底使用哪个基类的已有实现，编译器就会报错。还有后文中提到的基类与派生类之间的类型转化问题。解决编译错误的方法就是使用虚继承。

C\++标准库中的IO就用到了虚继承：

```c
class _LIBCPP_TYPE_VIS ios_base
{
    ...
};

template <class _CharT, class _Traits>
class _LIBCPP_TEMPLATE_VIS basic_ios
    : public ios_base
{
    ...
};

template <class _CharT, class _Traits>
class _LIBCPP_TEMPLATE_VIS basic_istream
    : virtual public basic_ios<_CharT, _Traits>
{
    ...
};

template <class _CharT, class _Traits>
class _LIBCPP_TEMPLATE_VIS basic_ostream
    : virtual public basic_ios<_CharT, _Traits>
{
    ...
}

template <class _CharT, class _Traits>
class _LIBCPP_TEMPLATE_VIS basic_iostream
    : public basic_istream<_CharT, _Traits>,
      public basic_ostream<_CharT, _Traits>
{
    ...
};
```

继承结构如下图：

```
graph TB
ios_base-->basic_ios
basic_ios-->basic_istream
basic_ios-->basic_ostream
basic_istream-->basic_iostream
basic_ostream-->basic_iostream
```

其中各个类常见的名字是：

```c
// char版本，还有一个wchar_t版本
typedef basic_istream<char>          istream;
typedef basic_ostream<char>          ostream;
typedef basic_iostream<char>         iostream;

// 而istream和ostream分别是：
extern _LIBCPP_FUNC_VIS istream cin;
extern _LIBCPP_FUNC_VIS ostream cout;
// 还有
extern _LIBCPP_FUNC_VIS ostream cerr;
extern _LIBCPP_FUNC_VIS ostream clog;
```

我们很少使用basic_iostream，也就是iostream。

## 虚基类的初始化

虚基类的初始化任务最终是由派生类完成的。虚基类永远被认为是其派生类的直接基类。

```c
struct V {
    V(int);
};

struct A {
    A();
};

struct B : virtual V, virtual A {
    B() : V{1} {/*...*/}    // 必须显式初始化虚基类V
};

class C : virtual V {
    C(int i) : V{i} { ... } // 必须显式初始化虚基类V
};

class D : virtual public V, virtual public C {
    // 从B，C隐式地继承虚基类V
    // 从B隐式地继承虚基类A
public: 
    D() { /*...*/}                      // error：C，V没有默认构造函数
    D(int i) : C{i} { ... }             // error: V没有默认构造函数
    D(int i, int j) : V{i}, C{j}{...}   // OK
};
```

上面D必须给V提供一个初始化，就像是D直接继承（不论是不是直接继承）自V一样。而且在构造D的时候，V的构造函数调用是自D中调用的，直接忽略B，C的构造函数，且与一般继承结构类似，先构造基类，这里是先构造虚基类。

## 虚基类的成员调用

一个虚基类的派生类在实现继承的接口时，不知道未来是否会有其他派生类实现该虚基类。在真正实现自身接口的时候，可以通过添加限定符直接调用虚基类的接口，而且这个过程没有用到虚调用机制。

菱形继承的虚继承在使用的时候，不能从派生类隐式转化为虚基类，因为搞不清楚是哪条派生路径，这决定了使用那个派生路径的具体实现，所以编译器需要搞清楚，从而出现二义性。相反地，从虚基类向派生类转化也是不行的。

如果两个派生自同一个基类的类都实现了同一个接口，但是这这两个派生类没有接口覆盖关系，那么这两个派生类作为基类派生一个新的类的时候，编译器会认为是一个错误，除非最终的派生类覆盖这个接口。

还有，虚继承会产生一个相对于常规继承而言比较“怪异”的调用行为，参考下面：

```c
// 一个被描述为 "delegate to a sister class" via virtual inheritance 的继承结构

class Base {
public:
  virtual void foo() = 0;
  virtual void bar() = 0;
};

class Der1 : public virtual Base {
public:
  virtual void foo();
};

void Der1::foo()
{ 
    bar(); 
}

class Der2 : public virtual Base {
public:
  virtual void bar(){
        std::cout << "in Der2::bar()" << std::endl;
    }
};

class Join : public Der1, public Der2 {
public:
  // ...
};

int main()
{
  Join* p1 = new Join();
  Der1* p2 = p1;
  Base* p3 = p1;
  p1->foo();
  p2->foo();
  p3->foo();
}

// 输出：
in Der2::bar()
in Der2::bar()
in Der2::bar()
```

Der1::foo() 调用 this->bar()，最终调用的是Der2::bar()；这个叫做“cross delegation”，可用来做一些特殊的自定义多态行为。这个是跟虚继承的内存结构相关的。

## 几种虚继承

下面两种继承结构，前者使用虚继承，后者不使用虚继承。

```c
class BB_ival_slider : public virtual Ival_slider, protected BBSlider {};
class Popup_ival_slider : public virtual Ival_slider {};
class BB_popup_ival_slider : public virtual Popup_ival_slider, protected BB_ival_slider {};


class BB_ival_slider : public Ival_slider, protected BBSlider {};
class Popup_ival_slider : public Ival_slider {};
class BB_popup_ival_slider : public Popup_ival_slider, protected BB_ival_slider {};
```

对比两种方案，虚基类的方案在派生类隐式转换为虚基类，以及相反的转换时有二义性问题；而后者没有问题。

另外，非虚基类的方案生成的对象空间占用较小（不需要数据结构来支持共享）。参考reference 1，可以看到虚继承在实现的时候，相比一般的继承，出现了一个新的辅助工具vbase_offset，用来记录虚函数具体使用哪一个入口。

## 虚继承与访问控制的关系

virtual可以与public、protected、private配合出现，二者组合顺序无所谓virtual public == public virtual。

## 几个问题

那么，std::iostream如何使用呢？好像很少见到使用的场景。一般单独使用cin和cout。

# C++ 对象内存结构

下面主要通过几个命令观察一下 C++ 类对象在不同情况下成员变量，以及虚表和虚指针的内存布局：

*clang 使用命令 `-fdump-vtable-layouts` 可以观察虚表结构，使用 `-fdump-record-layouts` 可以观察对象内存结构。下面代码使用这二者可以分别观察：观察到内存结构的变化。*

## 最简单的类

```C++
class Base {
public:
    Base() {}
    int a = 0;
    int b = 0;
};
int main()
{
    Base b;
    return 0;
}
```

没有虚表和虚指针，只能看到一个对象成员变量的内存结构：

```C++
//clang -cc1 -std=c++11 -stdlib=libc++ -fdump-vtable-layouts -fdump-record-layouts -emit-llvm ./test.cpp

*** Dumping AST Record Layout
         0 | class Base
         0 |   int a
         4 |   int b
           | [sizeof=8, dsize=8, align=4,
           |  nvsize=8, nvalign=4]
```

## 最简单的继承

```c++
class Base {
public:
    Base() {}
    int a = 0;
    int b = 0;
};
class Derived : public Base {
public:
    Derived() :Base() {}

    char c = 0;
    int d = 0;
};
int main()
{
    Derived d;
    return 0;
}
```

对应的内存结构是：

```c++
//clang -cc1 -std=c++11 -stdlib=libc++ -fdump-record-layouts  -emit-llvm ./test.cpp

*** Dumping AST Record Layout
         0 | class Base
         0 |   int a
         4 |   int b
           | [sizeof=8, dsize=8, align=4,
           |  nvsize=8, nvalign=4]

*** Dumping AST Record Layout
         0 | class Derived
         0 |   class Base (base)
         0 |     int a
         4 |     int b
         8 |   char c
        12 |   int d
           | [sizeof=16, dsize=16, align=4,
           |  nvsize=16, nvalign=4]
...
```

可以看到子类的内存结构就是在父类的结尾按序排列的。

## 最简单的虚类

```C++
class Base {
public:
    Base() {}
    virtual void test() {}

    int a = 0;
    int b = 0;
};

int main()
{
    Base b;
    return 0;
}
```

虚表的结构如下：

```C++
// clang -cc1 -std=c++11 -stdlib=libc++ -fdump-vtable-layouts -fdump-record-layouts -emit-llvm ./test.cpp

*** Dumping AST Record Layout
         0 | class Base
         0 |   (Base vtable pointer) // 这里加了一个虚指针，位于对象的首地址
         8 |   int a                 // 然后是成员变量的内存布局 
        12 |   int b
           | [sizeof=16, dsize=16, align=8,
           |  nvsize=16, nvalign=8]
...

Original map
Vtable for 'Base' (3 entries).
   0 | offset_to_top (0) // 这里记录当前虚类相对于第一个继承的父类对象虚表基地址的偏移，这里没有父类，偏移为0
   1 | Base RTTI
       -- (Base, 0) vtable address --
   2 | void Base::test()

VTable indices for 'Base' (1 entries).
   0 | void Base::test()
```

## 虚类派生一个子类

```c++
class Base {
public:
    Base() {}
    int a = 0;
    int b = 0;

    virtual void virtual_fun_in_Base() {}
};

class Derived : public Base {
public:
    Derived() :Base() {}

    char c = 0;
    int d = 0;
};

int main()
{
    Derived d;
    return 0;
}
```

输出内存变成了：

```c++
// clang -cc1 -std=c++11 -stdlib=libc++ -fdump-vtable-layouts -fdump-record-layouts -emit-llvm ./test.cpp

*** Dumping AST Record Layout
         0 | class Base
         0 |   (Base vtable pointer)
         8 |   int a
        12 |   int b
           | [sizeof=16, dsize=16, align=8,
           |  nvsize=16, nvalign=8]

*** Dumping AST Record Layout
         0 | class Derived
         0 |   class Base (primary base)
         0 |     (Base vtable pointer)
         8 |     int a
        12 |     int b
        16 |   char c
        20 |   int d
           | [sizeof=24, dsize=24, align=8,
           |  nvsize=24, nvalign=8]
...
// 下面是虚表
Original map
Vtable for 'Derived' (3 entries).
   0 | offset_to_top (0)
   1 | Derived RTTI
       -- (Base, 0) vtable address --
       -- (Derived, 0) vtable address --
   2 | void Base::virtual_fun_in_Base()


Original map
Vtable for 'Base' (3 entries).
   0 | offset_to_top (0)
   1 | Base RTTI
       -- (Base, 0) vtable address --
   2 | void Base::virtual_fun_in_Base()

VTable indices for 'Base' (1 entries).
   0 | void Base::virtual_fun_in_Base()
```

看一下子类 `Derived` 虚表中，保存了一个父类的虚表地址 `-- (Base, 0) vtable address --`，同时还有一个自己的地址：`-- (Derived, 0) vtable address --`。

## 派生类重载基类的虚函数

```C++
class Base {
public:
    Base() {}
    int a = 0;
    int b = 0;
    virtual void virtual_fun_in_Base() {
    }
};

class Derived : public Base {
public:
    Derived() :Base() {}

    char c = 0;
    int d = 0;

    virtual void virtual_fun_in_Base() override {
    }
    virtual void virtual_fun_in_Derived() {
    }
};
```

对应的内存布局：

```C++
// clang -cc1 -std=c++11 -stdlib=libc++ -fdump-vtable-layouts -fdump-record-layouts -emit-llvm ./test.cpp -o test

*** Dumping AST Record Layout
         0 | class Base
         0 |   (Base vtable pointer)
         8 |   int a
        12 |   int b
           | [sizeof=16, dsize=16, align=8,
           |  nvsize=16, nvalign=8]

*** Dumping AST Record Layout
         0 | class Derived
         0 |   class Base (primary base)
         0 |     (Base vtable pointer)
         8 |     int a
        12 |     int b
        16 |   char c
        20 |   int d
           | [sizeof=24, dsize=24, align=8,
           |  nvsize=24, nvalign=8]
...
// 下面是虚表
Original map
 void Derived::virtual_fun_in_Base() -> void Base::virtual_fun_in_Base() // 子类的这个方法指向了父类的方法（虚函数出现重载，才会有这一行）
Vtable for 'Derived' (3 entries).
   0 | offset_to_top (0)
   1 | Derived RTTI
       -- (Base, 0) vtable address --
       -- (Derived, 0) vtable address --
   2 | void Derived::virtual_fun_in_Base()

VTable indices for 'Derived' (1 entries).
   0 | void Derived::virtual_fun_in_Base()

Original map
 void Derived::virtual_fun_in_Base() -> void Base::virtual_fun_in_Base()
Vtable for 'Base' (3 entries).
   0 | offset_to_top (0)
   1 | Base RTTI
       -- (Base, 0) vtable address --
   2 | void Base::virtual_fun_in_Base()

VTable indices for 'Base' (1 entries).
   0 | void Base::virtual_fun_in_Base()
```

对比一下可以发现，`Derived` 虚表中的 `virtual_fun_in_Base` 方法，从之前的 `Base::virtual_fun_in_Base()` 变成了 `Derived::virtual_fun_in_Base()`。

## 派生类新增一个虚函数

```C++
class Derived : public Base {
public:
    ...
    virtual void virtual_fun_in_Derived() {}
};
```

输出：

```C++
// clang -cc1 -std=c++11 -stdlib=libc++ -fdump-vtable-layouts -fdump-record-layouts -emit-llvm ./test.cpp
*** Dumping AST Record Layout
         0 | class Base
         0 |   (Base vtable pointer)
         8 |   int a
        12 |   int b
           | [sizeof=16, dsize=16, align=8,
           |  nvsize=16, nvalign=8]

*** Dumping AST Record Layout
         0 | class Derived
         0 |   class Base (primary base)
         0 |     (Base vtable pointer)      // 子类对象内存布局首地址仍然是虚指针
         8 |     int a
        12 |     int b
        16 |   char c
        20 |   int d
           | [sizeof=24, dsize=24, align=8,
           |  nvsize=24, nvalign=8]

...

Original map
 void Derived::virtual_fun_in_Base() -> void Base::virtual_fun_in_Base() // 子类重载的方法指向父类的方法
Vtable for 'Derived' (4 entries).
   0 | offset_to_top (0)
   1 | Derived RTTI
       -- (Base, 0) vtable address --
       -- (Derived, 0) vtable address --
   2 | void Derived::virtual_fun_in_Base()
   3 | void Derived::virtual_fun_in_Derived()

VTable indices for 'Derived' (2 entries).       // 子类的虚表，包含两个虚函数的入口
   0 | void Derived::virtual_fun_in_Base()
   1 | void Derived::virtual_fun_in_Derived()

Original map
 void Derived::virtual_fun_in_Base() -> void Base::virtual_fun_in_Base()
Vtable for 'Base' (3 entries).
   0 | offset_to_top (0)                        // 首地址仍然偏移 0
   1 | Base RTTI
       -- (Base, 0) vtable address --
   2 | void Base::virtual_fun_in_Base()

VTable indices for 'Base' (1 entries).         // 父类的虚表中，只有父类的虚函数入口
   0 | void Base::virtual_fun_in_Base()
```

思考一下可以想到，如果子类对象构建的时候，直接把基类的虚指针指向子类的虚表即可。

## 非虚基类，派生类新增虚函数

```C++
class Base {
public:
    Base() {}
    int a = 0;
    int b = 0;
};

class Derived : public Base {
public:
    Derived() :Base() {}

    char c = 0;
    int d = 0;

    virtual void virtual_fun_in_Derived() {}
};
```

输出：

```C++
// clang -cc1 -std=c++11 -stdlib=libc++ -fdump-vtable-layouts -fdump-record-layouts -emit-llvm ./test.cpp

*** Dumping AST Record Layout
         0 | class Base
         0 |   int a                        // 父类这里没有虚指针
         4 |   int b
           | [sizeof=8, dsize=8, align=4,
           |  nvsize=8, nvalign=4]

*** Dumping AST Record Layout
         0 | class Derived
         0 |   (Derived vtable pointer)     // 子类这里有一个虚指针
         8 |   class Base (base)            // 然后才是成员变量的布局
         8 |     int a
        12 |     int b
        16 |   char c
        20 |   int d
           | [sizeof=24, dsize=24, align=8,
           |  nvsize=24, nvalign=8]
...
Original map
Vtable for 'Derived' (3 entries).
   0 | offset_to_top (0)
   1 | Derived RTTI
       -- (Derived, 0) vtable address --    // 只有子类虚表地址
   2 | void Derived::virtual_fun_in_Derived()

VTable indices for 'Derived' (1 entries).
   0 | void Derived::virtual_fun_in_Derived()
```

## 多继承

### 一个父类为非虚类，一个为虚类

```C++
class Base {
public:
    Base() {}
    int a = 0;
    int b = 0;
};

class Derived : public Base {
public:
    Derived() :Base() {}

    char c = 0;
    int d = 0;

    virtual void virtual_fun_in_Derived() {
    }
    virtual void virtual_fun_in_Derived_v2() {
    }
};


class Base2 {
public:
    virtual void virtual_fun_in_Base2() {}

    int e = 0;
};

class Derived2 : public Base, public Base2 {
public:
    virtual void virtual_fun_in_Derived2() {} // 新增一个虚函数
    double f = 0.0;
};
```

```C++
// clang -cc1 -std=c++11 -stdlib=libc++ -fdump-vtable-layouts -fdump-record-layouts -emit-llvm ./test.cpp

*** Dumping AST Record Layout
         0 | class Base
         0 |   int a
         4 |   int b
           | [sizeof=8, dsize=8, align=4,
           |  nvsize=8, nvalign=4]

*** Dumping AST Record Layout
         0 | class Base2
         0 |   (Base2 vtable pointer)
         8 |   int e
           | [sizeof=16, dsize=12, align=8,
           |  nvsize=12, nvalign=8]

*** Dumping AST Record Layout
         0 | class Derived2
         0 |   class Base2 (primary base)   // 内存布局中，首地址为虚父类 Base2
         0 |     (Base2 vtable pointer)     // 然后是Base2的虚指针
         8 |     int e                      // Base2 的成员变量
        12 |   class Base (base)            // 然后是另一个父类 Base
        12 |     int a
        16 |     int b
        24 |   double f                     // 最后是子类 Derived2 的成员变量
           | [sizeof=32, dsize=32, align=8,
           |  nvsize=32, nvalign=8]
...
Original map
Vtable for 'Derived2' (3 entries).
   0 | offset_to_top (0)
   1 | Derived2 RTTI
       -- (Base2, 0) vtable address --      // 子类的虚表地址，跟前面的一样，同时包含了 Base2 和 Derived2
       -- (Derived2, 0) vtable address --
   2 | void Base2::virtual_fun_in_Base2()   // 子类没有重载父类的方法，所以这里是父类的函数地址
   3 | void Derived2::virtual_fun_in_Derived2()

VTable indices for 'Derived2' (1 entries).
   1 | void Derived2::virtual_fun_in_Derived2()

Original map
Vtable for 'Base2' (3 entries).
   0 | offset_to_top (0)
   1 | Base2 RTTI
       -- (Base2, 0) vtable address --
   2 | void Base2::virtual_fun_in_Base2()

VTable indices for 'Base2' (1 entries).
   0 | void Base2::virtual_fun_in_Base2()
```

### 一个父类为虚类，另一个父类也是虚类

```C++
class Base {
public:
    Base() {}
    int a = 0;
    int b = 0;
    virtual void virtual_fun_in_Base() {}
};

class Base2 {
public:
    virtual void virtual_fun_in_Base2() {}

    int e = 0;
};

class Derived2 : public Base, public Base2 {
public:
    virtual void virtual_fun_in_Base2() override {}
    virtual void virtual_fun_in_Derived2() {}
    double f = 0.0;
};
```

输出： 

```C++
// clang -cc1 -std=c++11 -stdlib=libc++ -fdump-vtable-layouts -fdump-record-layouts -emit-llvm ./test.cpp
*** Dumping AST Record Layout
         0 | class Base
         0 |   (Base vtable pointer)
         8 |   int a
        12 |   int b
           | [sizeof=16, dsize=16, align=8,
           |  nvsize=16, nvalign=8]

*** Dumping AST Record Layout
         0 | class Base2
         0 |   (Base2 vtable pointer)
         8 |   int e
           | [sizeof=16, dsize=12, align=8,
           |  nvsize=12, nvalign=8]

*** Dumping AST Record Layout
         0 | class Derived2
         0 |   class Base (primary base)
         0 |     (Base vtable pointer)
         8 |     int a
        12 |     int b
        16 |   class Base2 (base)
        16 |     (Base2 vtable pointer)
        24 |     int e
        32 |   double f
           | [sizeof=40, dsize=40, align=8,
           |  nvsize=40, nvalign=8]

...

Original map
Vtable for 'Derived2' (8 entries).
   0 | offset_to_top (0)
   1 | Derived2 RTTI
       -- (Base, 0) vtable address --           // 子类的虚表地址中第一个地址为第一个（虚类）父类 Base
       -- (Derived2, 0) vtable address --
   2 | void Base::virtual_fun_in_Base()         // 然后是虚类 Base 的第一个方法
   3 | void Derived2::virtual_fun_in_Base2()    // 子类自身的方法接着按序排列
   4 | void Derived2::virtual_fun_in_Derived2()
   5 | offset_to_top (-16)                      // 出现 offset_to_top 非0值，这里表示另一个虚父类的开始
   6 | Derived2 RTTI                            // 第二个虚父类的信息
       -- (Base2, 16) vtable address --
   7 | void Derived2::virtual_fun_in_Base2()    // 第二个父类的虚函数
       [this adjustment: -16 non-virtual] method: void Base2::virtual_fun_in_Base2()

Thunks for 'void Derived2::virtual_fun_in_Base2()' (1 entry).
   0 | this adjustment: -16 non-virtual

VTable indices for 'Derived2' (2 entries).
   1 | void Derived2::virtual_fun_in_Base2()
   2 | void Derived2::virtual_fun_in_Derived2()

Original map
Vtable for 'Base' (3 entries).
   0 | offset_to_top (0)
   1 | Base RTTI
       -- (Base, 0) vtable address --
   2 | void Base::virtual_fun_in_Base()

VTable indices for 'Base' (1 entries).
   0 | void Base::virtual_fun_in_Base()

Original map
Vtable for 'Base2' (3 entries).
   0 | offset_to_top (0)
   1 | Base2 RTTI
       -- (Base2, 0) vtable address --
   2 | void Base2::virtual_fun_in_Base2()

VTable indices for 'Base2' (1 entries).
   0 | void Base2::virtual_fun_in_Base2()
```

### 增加到3个父类看看

```C++
// clang -cc1 -std=c++11 -stdlib=libc++ -fdump-vtable-layouts -fdump-record-layouts -emit-llvm ./test.cpp
class Base {
public:
    Base() {}
    int a = 0;
    int b = 0;
    virtual void virtual_fun_in_Base() {}
};

class Base2 {
public:
    virtual void virtual_fun_in_Base2() {}

    int e = 0;
};

class Base3 {
public:
    virtual void virtual_fun_in_Base3() {}

    int g = 0;
};

class Derived3 : public Base, public Base2, public Base3 {
public:
    virtual void virtual_fun_in_Derived3() {}
};
```

输出：

```C++
// clang -cc1 -std=c++11 -stdlib=libc++ -fdump-vtable-layouts -fdump-record-layouts -emit-llvm ./test.cpp

*** Dumping AST Record Layout
         0 | class Base
         0 |   (Base vtable pointer)
         8 |   int a
        12 |   int b
           | [sizeof=16, dsize=16, align=8,
           |  nvsize=16, nvalign=8]

*** Dumping AST Record Layout
         0 | class Base2
         0 |   (Base2 vtable pointer)
         8 |   int e
           | [sizeof=16, dsize=12, align=8,
           |  nvsize=12, nvalign=8]

*** Dumping AST Record Layout
         0 | class Base3
         0 |   (Base3 vtable pointer)
         8 |   int g
           | [sizeof=16, dsize=12, align=8,
           |  nvsize=12, nvalign=8]

*** Dumping AST Record Layout
         0 | class Derived3
         0 |   class Base (primary base)
         0 |     (Base vtable pointer)
         8 |     int a
        12 |     int b
        16 |   class Base2 (base)
        16 |     (Base2 vtable pointer)
        24 |     int e
        32 |   class Base3 (base)
        32 |     (Base3 vtable pointer)
        40 |     int g
           | [sizeof=48, dsize=44, align=8,
           |  nvsize=44, nvalign=8]

...

Original map
Vtable for 'Derived3' (10 entries).
   0 | offset_to_top (0)
   1 | Derived3 RTTI                            // 父类 Base 放在第一位
       -- (Base, 0) vtable address --
       -- (Derived3, 0) vtable address --
   2 | void Base::virtual_fun_in_Base()
   3 | void Derived3::virtual_fun_in_Derived3()
   4 | offset_to_top (-16)                      // 第二个虚父类 Base2
   5 | Derived3 RTTI                            // 接着是 Base2 的虚表入口
       -- (Base2, 16) vtable address --
   6 | void Base2::virtual_fun_in_Base2()
   7 | offset_to_top (-32)                      // 第三个虚父类
   8 | Derived3 RTTI                            // 接着是 Base3 的虚表入口
       -- (Base3, 32) vtable address --
   9 | void Base3::virtual_fun_in_Base3()

VTable indices for 'Derived3' (1 entries).
   1 | void Derived3::virtual_fun_in_Derived3()

Original map                                    // Base 的虚表结构
Vtable for 'Base' (3 entries).
   0 | offset_to_top (0)
   1 | Base RTTI
       -- (Base, 0) vtable address --
   2 | void Base::virtual_fun_in_Base()

VTable indices for 'Base' (1 entries).
   0 | void Base::virtual_fun_in_Base()

Original map                                    // Base2 的虚表结构
Vtable for 'Base2' (3 entries).
   0 | offset_to_top (0)
   1 | Base2 RTTI
       -- (Base2, 0) vtable address --
   2 | void Base2::virtual_fun_in_Base2()

VTable indices for 'Base2' (1 entries).
   0 | void Base2::virtual_fun_in_Base2()

Original map                                    // Base3 的虚表结构
Vtable for 'Base3' (3 entries).
   0 | offset_to_top (0)
   1 | Base3 RTTI
       -- (Base3, 0) vtable address --
   2 | void Base3::virtual_fun_in_Base3()

VTable indices for 'Base3' (1 entries).
   0 | void Base3::virtual_fun_in_Base3()
```

### 子类虚表中偏移地址

前面输出结果中 offset_to_top 的偏移地址看起来有些疑问，为什么偏移一个 16，一个 32 呢？

```C++
class Base {
public:
    Base() {}
    int a = 0;
    int b = 0;
    virtual void virtual_fun_in_Base() {}
};

class Base2 {
public:
    virtual void virtual_fun_in_Base2() {}

    int e = 0;
    int base2_1 = 0;    // 这里新增三个成员变量，增大 Base2 的内存占用空间
    int base2_2 = 0;
    int base2_3 = 0;
};

class Base3 {
public:
    virtual void virtual_fun_in_Base3() {}

    int g = 0;
};

class Derived3 : public Base, public Base2, public Base3 {
public:
    virtual void virtual_fun_in_Derived3() {}
};
```

输出：

```C++
clang -cc1 -std=c++11 -stdlib=libc++ -fdump-vtable-layouts -fdump-record-layouts -emit-llvm ./test.cpp

*** Dumping AST Record Layout
         0 | class Base
         0 |   (Base vtable pointer)
         8 |   int a
        12 |   int b
           | [sizeof=16, dsize=16, align=8,
           |  nvsize=16, nvalign=8]

*** Dumping AST Record Layout
         0 | class Base2
         0 |   (Base2 vtable pointer)
         8 |   int e
        12 |   int base2_1
        16 |   int base2_2
        20 |   int base2_3
           | [sizeof=24, dsize=24, align=8,     // 这里的 sizeof 从之前的16，变成了 24.
           |  nvsize=24, nvalign=8]

*** Dumping AST Record Layout
         0 | class Base3
         0 |   (Base3 vtable pointer)
         8 |   int g
           | [sizeof=16, dsize=12, align=8,
           |  nvsize=12, nvalign=8]

*** Dumping AST Record Layout
         0 | class Derived3
         0 |   class Base (primary base)
         0 |     (Base vtable pointer)
         8 |     int a
        12 |     int b
        16 |   class Base2 (base)
        16 |     (Base2 vtable pointer)
        24 |     int e
        28 |     int base2_1
        32 |     int base2_2
        36 |     int base2_3
        40 |   class Base3 (base)
        40 |     (Base3 vtable pointer)
        48 |     int g
           | [sizeof=56, dsize=52, align=8,
           |  nvsize=52, nvalign=8]
...

Original map
Vtable for 'Derived3' (10 entries).             // 子类虚表的布局
   0 | offset_to_top (0)
   1 | Derived3 RTTI
       -- (Base, 0) vtable address --
       -- (Derived3, 0) vtable address --
   2 | void Base::virtual_fun_in_Base()
   3 | void Derived3::virtual_fun_in_Derived3()
   4 | offset_to_top (-16)                      // 偏移地址还是 -16
   5 | Derived3 RTTI
       -- (Base2, 16) vtable address --
   6 | void Base2::virtual_fun_in_Base2()
   7 | offset_to_top (-40)                      // 偏移地址从之前的 -32 变成了现在的 -40，相比之前正好多了新增的3个成员变量的空间
   8 | Derived3 RTTI
       -- (Base3, 40) vtable address --
   9 | void Base3::virtual_fun_in_Base3()

VTable indices for 'Derived3' (1 entries).
   1 | void Derived3::virtual_fun_in_Derived3()

Original map
Vtable for 'Base' (3 entries).
   0 | offset_to_top (0)
   1 | Base RTTI
       -- (Base, 0) vtable address --
   2 | void Base::virtual_fun_in_Base()

VTable indices for 'Base' (1 entries).
   0 | void Base::virtual_fun_in_Base()

Original map
Vtable for 'Base2' (3 entries).
   0 | offset_to_top (0)
   1 | Base2 RTTI
       -- (Base2, 0) vtable address --
   2 | void Base2::virtual_fun_in_Base2()

VTable indices for 'Base2' (1 entries).
   0 | void Base2::virtual_fun_in_Base2()

Original map
Vtable for 'Base3' (3 entries).
   0 | offset_to_top (0)
   1 | Base3 RTTI
       -- (Base3, 0) vtable address --
   2 | void Base3::virtual_fun_in_Base3()

VTable indices for 'Base3' (1 entries).
   0 | void Base3::virtual_fun_in_Base3()
```

## 虚继承对象

### 先看一下不使用虚继承的菱形继承

```C++
class Base {
public:
    Base() {}
    int a = 0;
    int b = 0;
    virtual void virtual_fun_in_Base() {
        a = 1;
    }
};


class DerivedLeft :  public Base {
public:
    int left = 0;
};

class DerivedRight :  public Base {
public:
    int right = 0;
};

class Diamond : public DerivedLeft, public DerivedRight {
public:
};
```

```C++
// clang -cc1 -std=c++11 -stdlib=libc++ -fdump-vtable-layouts -fdump-record-layouts -emit-llvm ./test.cpp

*** Dumping AST Record Layout
         0 | class Base
         0 |   (Base vtable pointer)
         8 |   int a
        12 |   int b
           | [sizeof=16, dsize=16, align=8,
           |  nvsize=16, nvalign=8]

*** Dumping AST Record Layout
         0 | class DerivedLeft
         0 |   class Base (primary base)
         0 |     (Base vtable pointer)
         8 |     int a
        12 |     int b
        16 |   int left
           | [sizeof=24, dsize=20, align=8,
           |  nvsize=20, nvalign=8]

*** Dumping AST Record Layout
         0 | class DerivedRight
         0 |   class Base (primary base)
         0 |     (Base vtable pointer)
         8 |     int a
        12 |     int b
        16 |   int right
           | [sizeof=24, dsize=20, align=8,
           |  nvsize=20, nvalign=8]

*** Dumping AST Record Layout
         0 | class Diamond
         0 |   class DerivedLeft (primary base)     // 可以看到出现了 DerivedLeft 的内存结构
         0 |     class Base (primary base)          // Base 的内存结构出现了一次
         0 |       (Base vtable pointer)
         8 |       int a
        12 |       int b
        16 |     int left
        24 |   class DerivedRight (base)            // 可以看到出现了 DerivedRight 的内存结构
        24 |     class Base (primary base)          // Base 的内存结构重复出现了
        24 |       (Base vtable pointer)
        32 |       int a
        36 |       int b
        40 |     int right
           | [sizeof=48, dsize=44, align=8,
           |  nvsize=44, nvalign=8]
...
Original map
Vtable for 'Diamond' (6 entries).
   0 | offset_to_top (0)
   1 | Diamond RTTI
       -- (Base, 0) vtable address --
       -- (DerivedLeft, 0) vtable address --
       -- (Diamond, 0) vtable address --
   2 | void Base::virtual_fun_in_Base()
   3 | offset_to_top (-24)
   4 | Diamond RTTI
       -- (Base, 24) vtable address --
       -- (DerivedRight, 24) vtable address --
   5 | void Base::virtual_fun_in_Base()


Original map
Vtable for 'DerivedLeft' (3 entries).
   0 | offset_to_top (0)
   1 | DerivedLeft RTTI
       -- (Base, 0) vtable address --
       -- (DerivedLeft, 0) vtable address --
   2 | void Base::virtual_fun_in_Base()


Original map
Vtable for 'Base' (3 entries).
   0 | offset_to_top (0)
   1 | Base RTTI
       -- (Base, 0) vtable address --
   2 | void Base::virtual_fun_in_Base()

VTable indices for 'Base' (1 entries).
   0 | void Base::virtual_fun_in_Base()

Original map
Vtable for 'DerivedRight' (3 entries).
   0 | offset_to_top (0)
   1 | DerivedRight RTTI
       -- (Base, 0) vtable address --
       -- (DerivedRight, 0) vtable address --
   2 | void Base::virtual_fun_in_Base()
```

### 在看一下使用虚继承的菱形继承

```C++
class Base {
public:
    Base() {}
    int a = 0;
    int b = 0;
    virtual void virtual_fun_in_Base() {
        a = 1;
    }
};

class DerivedLeft : virtual public Base {
public:
    virtual void virtual_fun_in_Base() override {
        a = 2;
    }
};

class DerivedRight : virtual public Base {
public:
    virtual void virtual_fun_in_Base() override {
        a = 3;
    }
};

class Diamond : public DerivedLeft, public DerivedRight {
public:
    virtual void virtual_fun_in_Base() override {
        a = 4;
    }
};
```
内存布局以及虚表结构：

```C++
// clang -cc1 -std=c++11 -stdlib=libc++ -fdump-vtable-layouts -fdump-record-layouts -emit-llvm ./test.cpp

*** Dumping AST Record Layout
         0 | class Base
         0 |   (Base vtable pointer)
         8 |   int a
        12 |   int b
           | [sizeof=16, dsize=16, align=8,
           |  nvsize=16, nvalign=8]

*** Dumping AST Record Layout
         0 | class DerivedLeft
         0 |   (DerivedLeft vtable pointer)     // 虚继承的这个地方，出现了一个新的虚表地址
         8 |   class Base (virtual base)        // 然后是父类的内存布局
         8 |     (Base vtable pointer)
        16 |     int a
        20 |     int b
           | [sizeof=24, dsize=24, align=8,
           |  nvsize=8, nvalign=8]

*** Dumping AST Record Layout
         0 | class DerivedRight
         0 |   (DerivedRight vtable pointer)    // 同上
         8 |   class Base (virtual base)
         8 |     (Base vtable pointer)
        16 |     int a
        20 |     int b
           | [sizeof=24, dsize=24, align=8,
           |  nvsize=8, nvalign=8]

*** Dumping AST Record Layout
         0 | class Diamond
         0 |   class DerivedLeft (primary base) // 继承了虚继承类的子类，这里按序将父类做了排列，第一个类仍然是主父类
         0 |     (DerivedLeft vtable pointer)
         8 |   class DerivedRight (base)        // 第二个父类的虚表地址，当然也是第二个父类的基地址
         8 |     (DerivedRight vtable pointer)
        16 |   class Base (virtual base)        // 然后是最顶部的父类
        16 |     (Base vtable pointer)          // 这里父类的成员没有出现多次
        24 |     int a
        28 |     int b
           | [sizeof=32, dsize=32, align=8,
           |  nvsize=16, nvalign=8]
...
// 虚表结构
Original map                                    // 子类的虚表结构
 void Diamond::virtual_fun_in_Base() -> void DerivedLeft::virtual_fun_in_Base()
Vtable for 'Diamond' (12 entries).
   0 | vbase_offset (16)
   1 | offset_to_top (0)
   2 | Diamond RTTI
       -- (DerivedLeft, 0) vtable address --
       -- (Diamond, 0) vtable address --
   3 | void Diamond::virtual_fun_in_Base()
   4 | vbase_offset (8)
   5 | offset_to_top (-8)
   6 | Diamond RTTI
       -- (DerivedRight, 8) vtable address --
   7 | void Diamond::virtual_fun_in_Base()
       [this adjustment: -8 non-virtual] method: void DerivedRight::virtual_fun_in_Base()
   8 | vcall_offset (-16)
   9 | offset_to_top (-16)
  10 | Diamond RTTI
       -- (Base, 16) vtable address --
  11 | void Diamond::virtual_fun_in_Base()
       [this adjustment: 0 non-virtual, -24 vcall offset offset] method: void Base::virtual_fun_in_Base()

Virtual base offset offsets for 'Diamond' (1 entry).            // 首次新增的结构
   Base | -24

Thunks for 'void Diamond::virtual_fun_in_Base()' (2 entries).   // 首次新增的结构，使用了 Thunk 技术
   0 | this adjustment: -8 non-virtual
   1 | this adjustment: 0 non-virtual, -24 vcall offset offset

VTable indices for 'Diamond' (1 entries).
   0 | void Diamond::virtual_fun_in_Base()

Original map
 void Diamond::virtual_fun_in_Base() -> void DerivedLeft::virtual_fun_in_Base()
Construction vtable for ('DerivedLeft', 0) in 'Diamond' (8 entries).
   0 | vbase_offset (16)
   1 | offset_to_top (0)
   2 | DerivedLeft RTTI
       -- (DerivedLeft, 0) vtable address --
   3 | void DerivedLeft::virtual_fun_in_Base()
   4 | vcall_offset (-16)
   5 | offset_to_top (-16)
   6 | DerivedLeft RTTI
       -- (Base, 16) vtable address --
   7 | void DerivedLeft::virtual_fun_in_Base()
       [this adjustment: 0 non-virtual, -24 vcall offset offset] method: void Base::virtual_fun_in_Base()

Original map
 void Diamond::virtual_fun_in_Base() -> void DerivedLeft::virtual_fun_in_Base()
Construction vtable for ('DerivedRight', 8) in 'Diamond' (8 entries).
   0 | vbase_offset (8)
   1 | offset_to_top (0)
   2 | DerivedRight RTTI
       -- (DerivedRight, 8) vtable address --
   3 | void DerivedRight::virtual_fun_in_Base()
   4 | vcall_offset (-8)
   5 | offset_to_top (-8)
   6 | DerivedRight RTTI
       -- (Base, 16) vtable address --
   7 | void DerivedRight::virtual_fun_in_Base()
       [this adjustment: 0 non-virtual, -24 vcall offset offset] method: void Base::virtual_fun_in_Base()

Original map
 void Diamond::virtual_fun_in_Base() -> void DerivedLeft::virtual_fun_in_Base()
Vtable for 'Base' (3 entries).
   0 | offset_to_top (0)
   1 | Base RTTI
       -- (Base, 0) vtable address --
   2 | void Base::virtual_fun_in_Base()

VTable indices for 'Base' (1 entries).
   0 | void Base::virtual_fun_in_Base()

Original map
 void Diamond::virtual_fun_in_Base() -> void DerivedLeft::virtual_fun_in_Base()
Vtable for 'DerivedLeft' (8 entries).
   0 | vbase_offset (8)
   1 | offset_to_top (0)
   2 | DerivedLeft RTTI
       -- (DerivedLeft, 0) vtable address --
   3 | void DerivedLeft::virtual_fun_in_Base()
   4 | vcall_offset (-8)
   5 | offset_to_top (-8)
   6 | DerivedLeft RTTI
       -- (Base, 8) vtable address --
   7 | void DerivedLeft::virtual_fun_in_Base()
       [this adjustment: 0 non-virtual, -24 vcall offset offset] method: void Base::virtual_fun_in_Base()

Virtual base offset offsets for 'DerivedLeft' (1 entry).
   Base | -24

Thunks for 'void DerivedLeft::virtual_fun_in_Base()' (1 entry).
   0 | this adjustment: 0 non-virtual, -24 vcall offset offset

VTable indices for 'DerivedLeft' (1 entries).
   0 | void DerivedLeft::virtual_fun_in_Base()

Original map
 void Diamond::virtual_fun_in_Base() -> void DerivedLeft::virtual_fun_in_Base()
Vtable for 'DerivedRight' (8 entries).
   0 | vbase_offset (8)
   1 | offset_to_top (0)
   2 | DerivedRight RTTI
       -- (DerivedRight, 0) vtable address --
   3 | void DerivedRight::virtual_fun_in_Base()
   4 | vcall_offset (-8)
   5 | offset_to_top (-8)
   6 | DerivedRight RTTI
       -- (Base, 8) vtable address --
   7 | void DerivedRight::virtual_fun_in_Base()
       [this adjustment: 0 non-virtual, -24 vcall offset offset] method: void Base::virtual_fun_in_Base()

Virtual base offset offsets for 'DerivedRight' (1 entry).
   Base | -24

Thunks for 'void DerivedRight::virtual_fun_in_Base()' (1 entry).
   0 | this adjustment: 0 non-virtual, -24 vcall offset offset

VTable indices for 'DerivedRight' (1 entries).
   0 | void DerivedRight::virtual_fun_in_Base()
```

*对于堆来讲，生长方向是向上的，也就是向着内存地址增加的方向；对于栈来讲，它的生长方式是向下的，是向着内存地址减小的方向增长。*


# reference

1. [C++中虚函数、虚继承内存模型](https://zhuanlan.zhihu.com/p/41309205) 介绍了虚继承的C\++对象内存布局
2. [C.129: When designing a class hierarchy, distinguish between implementation inheritance and interface inheritance](https://www.modernescpp.com/index.php/c-core-guidelines-more-rules-to-class-hierarchies)
3. [C++ Decorate basic_iostream classes](https://stackoverflow.com/questions/6384860/c-decorate-basic-iostream-classes) 教你如何继承std::basic_streambuf和std::basic_ostream来实现自己的标准输出。
4. [【C++拾遗】 从内存布局看C++虚继承的实现原理](https://blog.csdn.net/Xiejingfa/article/details/48028491)
5. [Solving the Diamond Problem with Virtual Inheritance](https://www.cprogramming.com/tutorial/virtual_inheritance.html)
6. [What is the “dreaded diamond”?](https://isocpp.org/wiki/faq/multiple-inheritance#mi-diamond) 这是isocpp上的FAQ，该问答之后的几个问答都非常好。
7. [Dumping a C++ object's memory layout with Clang](https://eli.thegreenplace.net/2012/12/17/dumping-a-c-objects-memory-layout-with-clang/)