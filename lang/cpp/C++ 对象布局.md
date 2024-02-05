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
3. 虚表放在了其他地方，仅需要虚指针可以找到即可，所以不占用对象大小，参考Derived2::$vftable@:和Derived3::$vftable@:。
4. 虚基类和非虚基类同时被继承的话，内存上先做虚基类的布局。参考Derived2。

# C++ 虚继承以及内存布局

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

## 看一个例子，观察虚继承对象内存结构

写一个示例代码test.cpp，使用clang查看内存结构：

查看下面的数据的时候，要注意：对于堆来讲，生长方向是向上的，也就是向着内存地址增加的方向；对于栈来讲，它的生长方式是向下的，是向着内存地址减小的方向增长。

```c
struct A
{
    int ax;
    virtual void f0() {}
    virtual void bar() {}
};

struct B : virtual public A
{
    int bx;
    void f0() {}
};

struct C : virtual public A
{
    int cx;
    void f0() {}
};

struct D : public B, public C
{
    int dx;
    void f0() {}
};

int main()
{
    D d;
    return 0;
}
```

使用命令：clang -cc1 -stdlib=libc++ -fdump-record-layouts -fdump-vtable-layouts -emit-llvm ./test.cpp

```c
*** Dumping AST Record Layout
         0 | struct A
         0 |   (A vtable pointer)
         8 |   int ax
           | [sizeof=16, dsize=12, align=8,
           |  nvsize=12, nvalign=8]

*** Dumping AST Record Layout
         0 | struct B
         0 |   (B vtable pointer)
         8 |   int bx
        16 |   struct A (virtual base)
        16 |     (A vtable pointer)
        24 |     int ax
           | [sizeof=32, dsize=28, align=8,
           |  nvsize=12, nvalign=8]

*** Dumping AST Record Layout
         0 | struct C
         0 |   (C vtable pointer)
         8 |   int cx
        16 |   struct A (virtual base)
        16 |     (A vtable pointer)
        24 |     int ax
           | [sizeof=32, dsize=28, align=8,
           |  nvsize=12, nvalign=8]

*** Dumping AST Record Layout
         0 | struct D
         0 |   struct B (primary base)
         0 |     (B vtable pointer)
         8 |     int bx
        16 |   struct C (base)
        16 |     (C vtable pointer)
        24 |     int cx
        28 |   int dx
        32 |   struct A (virtual base)
        32 |     (A vtable pointer)
        40 |     int ax
           | [sizeof=48, dsize=44, align=8,
           |  nvsize=32, nvalign=8]

*** Dumping IRgen Record Layout
Record: CXXRecordDecl 0x7f8398078700 <./test.cpp:1:1, line:6:1> line:1:8 referenced struct A definition
|-DefinitionData polymorphic
| |-DefaultConstructor exists non_trivial
| |-CopyConstructor simple non_trivial has_const_param implicit_has_const_param
| |-MoveConstructor exists simple non_trivial needs_implicit
| |-CopyAssignment non_trivial has_const_param implicit_has_const_param
| |-MoveAssignment exists simple non_trivial needs_implicit
| `-Destructor simple irrelevant trivial
|-CXXRecordDecl 0x7f8398078818 <col:1, col:8> col:8 implicit struct A
|-FieldDecl 0x7f83980788c0 <line:3:5, col:9> col:9 ax 'int'
|-CXXMethodDecl 0x7f8398078958 <line:4:5, col:24> col:18 f0 'void ()' virtual
| `-CompoundStmt 0x7f8398078db8 <col:23, col:24>
|-CXXMethodDecl 0x7f8398078a18 <line:5:5, col:25> col:18 bar 'void ()' virtual
| `-CompoundStmt 0x7f8398078dc8 <col:24, col:25>
|-CXXMethodDecl 0x7f8398078b18 <line:1:8> col:8 implicit operator= 'A &(const A &)' inline default noexcept-unevaluated 0x7f8398078b18
| `-ParmVarDecl 0x7f8398078c28 <col:8> col:8 'const A &'
|-CXXDestructorDecl 0x7f8398078cb0 <col:8> col:8 implicit ~A 'void ()' inline default trivial noexcept-unevaluated 0x7f8398078cb0
|-CXXConstructorDecl 0x7f83980a3ef8 <col:8> col:8 implicit used A 'void () throw()' inline default
| `-CompoundStmt 0x7f83980a4278 <col:8>
`-CXXConstructorDecl 0x7f83980a40f0 <col:8> col:8 implicit A 'void (const A &)' inline default noexcept-unevaluated 0x7f83980a40f0
  `-ParmVarDecl 0x7f83980a4208 <col:8> col:8 'const A &'

Layout: <CGRecordLayout
  LLVMType:%struct.A = type <{ i32 (...)**, i32, [4 x i8] }>
  NonVirtualBaseLLVMType:%struct.A.base = type <{ i32 (...)**, i32 }>
  IsZeroInitializable:1
  BitFields:[
]>

*** Dumping IRgen Record Layout
Record: CXXRecordDecl 0x7f8398078dd8 <./test.cpp:8:1, line:12:1> line:8:8 referenced struct B definition
|-DefinitionData polymorphic
| |-DefaultConstructor exists non_trivial
| |-CopyConstructor simple non_trivial has_const_param implicit_has_const_param
| |-MoveConstructor exists simple non_trivial needs_implicit
| |-CopyAssignment non_trivial has_const_param implicit_has_const_param
| |-MoveAssignment exists simple non_trivial needs_implicit
| `-Destructor simple irrelevant trivial
|-virtual public 'A'
|-CXXRecordDecl 0x7f8398078f50 <col:1, col:8> col:8 implicit struct B
|-FieldDecl 0x7f8398078ff8 <line:10:5, col:9> col:9 bx 'int'
|-CXXMethodDecl 0x7f8398079068 <line:11:5, col:16> col:10 f0 'void ()'
| |-Overrides: [ 0x7f8398078958 A::f0 'void ()' ]
| `-CompoundStmt 0x7f8398079408 <col:15, col:16>
|-CXXMethodDecl 0x7f8398079168 <line:8:8> col:8 implicit operator= 'B &(const B &)' inline default noexcept-unevaluated 0x7f8398079168
| `-ParmVarDecl 0x7f8398079278 <col:8> col:8 'const B &'
|-CXXDestructorDecl 0x7f8398079300 <col:8> col:8 implicit ~B 'void ()' inline default trivial noexcept-unevaluated 0x7f8398079300
|-CXXConstructorDecl 0x7f83980a3e00 <col:8> col:8 implicit used B 'void () throw()' inline default
| |-CXXCtorInitializer 'A'
| | `-CXXConstructExpr 0x7f83980a4468 <col:8> 'A' 'void () throw()'
| `-CompoundStmt 0x7f83980a44d0 <col:8>
`-CXXConstructorDecl 0x7f83980a42e8 <col:8> col:8 implicit B 'void (const B &)' inline default noexcept-unevaluated 0x7f83980a42e8
  `-ParmVarDecl 0x7f83980a43f8 <col:8> col:8 'const B &'

Layout: <CGRecordLayout
  LLVMType:%struct.B = type <{ i32 (...)**, i32, [4 x i8], %struct.A.base, [4 x i8] }>
  NonVirtualBaseLLVMType:%struct.B.base = type <{ i32 (...)**, i32 }>
  IsZeroInitializable:1
  BitFields:[
]>

*** Dumping IRgen Record Layout
Record: CXXRecordDecl 0x7f8398079418 <./test.cpp:14:1, line:18:1> line:14:8 referenced struct C definition
|-DefinitionData polymorphic
| |-DefaultConstructor exists non_trivial
| |-CopyConstructor simple non_trivial has_const_param implicit_has_const_param
| |-MoveConstructor exists simple non_trivial needs_implicit
| |-CopyAssignment non_trivial has_const_param implicit_has_const_param
| |-MoveAssignment exists simple non_trivial needs_implicit
| `-Destructor simple irrelevant trivial
|-virtual public 'A'
|-CXXRecordDecl 0x7f83980a2200 <col:1, col:8> col:8 implicit struct C
|-FieldDecl 0x7f83980a22a8 <line:16:5, col:9> col:9 cx 'int'
|-CXXMethodDecl 0x7f83980a2318 <line:17:5, col:16> col:10 f0 'void ()'
| |-Overrides: [ 0x7f8398078958 A::f0 'void ()' ]
| `-CompoundStmt 0x7f83980a26b8 <col:15, col:16>
|-CXXMethodDecl 0x7f83980a2418 <line:14:8> col:8 implicit operator= 'C &(const C &)' inline default noexcept-unevaluated 0x7f83980a2418
| `-ParmVarDecl 0x7f83980a2528 <col:8> col:8 'const C &'
|-CXXDestructorDecl 0x7f83980a25b0 <col:8> col:8 implicit ~C 'void ()' inline default trivial noexcept-unevaluated 0x7f83980a25b0
|-CXXConstructorDecl 0x7f83980a4010 <col:8> col:8 implicit used C 'void () throw()' inline default
| |-CXXCtorInitializer 'A'
| | `-CXXConstructExpr 0x7f83980a46c8 <col:8> 'A' 'void () throw()'
| `-CompoundStmt 0x7f83980a4730 <col:8>
`-CXXConstructorDecl 0x7f83980a4540 <col:8> col:8 implicit C 'void (const C &)' inline default noexcept-unevaluated 0x7f83980a4540
  `-ParmVarDecl 0x7f83980a4658 <col:8> col:8 'const C &'

Layout: <CGRecordLayout
  LLVMType:%struct.C = type <{ i32 (...)**, i32, [4 x i8], %struct.A.base, [4 x i8] }>
  NonVirtualBaseLLVMType:%struct.C.base = type <{ i32 (...)**, i32 }>
  IsZeroInitializable:1
  BitFields:[
]>

*** Dumping IRgen Record Layout
Record: CXXRecordDecl 0x7f83980a26c8 <./test.cpp:20:1, line:24:1> line:20:8 referenced struct D definition
|-DefinitionData polymorphic
| |-DefaultConstructor exists non_trivial
| |-CopyConstructor simple non_trivial has_const_param implicit_has_const_param
| |-MoveConstructor exists simple non_trivial needs_implicit
| |-CopyAssignment non_trivial has_const_param implicit_has_const_param
| |-MoveAssignment exists simple non_trivial needs_implicit
| `-Destructor simple irrelevant trivial
|-public 'B'
|-public 'C'
|-CXXRecordDecl 0x7f83980a2880 <col:1, col:8> col:8 implicit struct D
|-FieldDecl 0x7f83980a2928 <line:22:5, col:9> col:9 dx 'int'
|-CXXMethodDecl 0x7f83980a2998 <line:23:5, col:16> col:10 f0 'void ()'
| |-Overrides: [ 0x7f8398079068 B::f0 'void ()', 0x7f83980a2318 C::f0 'void ()' ]
| `-CompoundStmt 0x7f83980a2d38 <col:15, col:16>
|-CXXMethodDecl 0x7f83980a2a98 <line:20:8> col:8 implicit operator= 'D &(const D &)' inline default noexcept-unevaluated 0x7f83980a2a98
| `-ParmVarDecl 0x7f83980a2ba8 <col:8> col:8 'const D &'
|-CXXDestructorDecl 0x7f83980a2c30 <col:8> col:8 implicit ~D 'void ()' inline default trivial noexcept-unevaluated 0x7f83980a2c30
|-CXXConstructorDecl 0x7f83980a2f40 <col:8> col:8 implicit used D 'void () throw()' inline default
| |-CXXCtorInitializer 'A'
| | `-CXXConstructExpr 0x7f83980a4288 <col:8> 'A' 'void () throw()'
| |-CXXCtorInitializer 'B'
| | `-CXXConstructExpr 0x7f83980a44e0 <col:8> 'B' 'void () throw()'
| |-CXXCtorInitializer 'C'
| | `-CXXConstructExpr 0x7f83980a4740 <col:8> 'C' 'void () throw()'
| `-CompoundStmt 0x7f83980a47b8 <col:8>
`-CXXConstructorDecl 0x7f83980a3020 <col:8> col:8 implicit D 'void (const D &)' inline default noexcept-unevaluated 0x7f83980a3020
  `-ParmVarDecl 0x7f83980a3138 <col:8> col:8 'const D &'

Layout: <CGRecordLayout
  LLVMType:%struct.D = type { %struct.B.base, [4 x i8], %struct.C.base, i32, %struct.A.base, [4 x i8] }
  NonVirtualBaseLLVMType:%struct.D.base = type { %struct.B.base, [4 x i8], %struct.C.base, i32 }
  IsZeroInitializable:1
  BitFields:[
]>
Original map
 void D::f0() -> void B::f0()
Vtable for 'D' (14 entries).
   0 | vbase_offset (32)
   1 | offset_to_top (0)
   2 | D RTTI
       -- (B, 0) vtable address --
       -- (D, 0) vtable address --
   3 | void D::f0()
   4 | vbase_offset (16)
   5 | offset_to_top (-16)
   6 | D RTTI
       -- (C, 16) vtable address --
   7 | void D::f0()
       [this adjustment: -16 non-virtual] method: void C::f0()
   8 | vcall_offset (0)
   9 | vcall_offset (-32)
  10 | offset_to_top (-32)
  11 | D RTTI
       -- (A, 32) vtable address --
  12 | void D::f0()
       [this adjustment: 0 non-virtual, -24 vcall offset offset] method: void A::f0()
  13 | void A::bar()

Virtual base offset offsets for 'D' (1 entry).
   A | -24

Thunks for 'void D::f0()' (2 entries).
   0 | this adjustment: -16 non-virtual
   1 | this adjustment: 0 non-virtual, -24 vcall offset offset

VTable indices for 'D' (1 entries).
   0 | void D::f0()

Original map
 void D::f0() -> void B::f0()
Construction vtable for ('B', 0) in 'D' (10 entries).
   0 | vbase_offset (32)
   1 | offset_to_top (0)
   2 | B RTTI
       -- (B, 0) vtable address --
   3 | void B::f0()
   4 | vcall_offset (0)
   5 | vcall_offset (-32)
   6 | offset_to_top (-32)
   7 | B RTTI
       -- (A, 32) vtable address --
   8 | void B::f0()
       [this adjustment: 0 non-virtual, -24 vcall offset offset] method: void A::f0()
   9 | void A::bar()

Original map
 void D::f0() -> void B::f0()
Construction vtable for ('C', 16) in 'D' (10 entries).
   0 | vbase_offset (16)
   1 | offset_to_top (0)
   2 | C RTTI
       -- (C, 16) vtable address --
   3 | void C::f0()
   4 | vcall_offset (0)
   5 | vcall_offset (-16)
   6 | offset_to_top (-16)
   7 | C RTTI
       -- (A, 32) vtable address --
   8 | void C::f0()
       [this adjustment: 0 non-virtual, -24 vcall offset offset] method: void A::f0()
   9 | void A::bar()

Original map
 void D::f0() -> void B::f0()
Vtable for 'A' (4 entries).
   0 | offset_to_top (0)
   1 | A RTTI
       -- (A, 0) vtable address --
   2 | void A::f0()
   3 | void A::bar()

VTable indices for 'A' (2 entries).
   0 | void A::f0()
   1 | void A::bar()

Original map
 void D::f0() -> void B::f0()
Vtable for 'B' (10 entries).
   0 | vbase_offset (16)
   1 | offset_to_top (0)
   2 | B RTTI
       -- (B, 0) vtable address --
   3 | void B::f0()
   4 | vcall_offset (0)
   5 | vcall_offset (-16)
   6 | offset_to_top (-16)
   7 | B RTTI
       -- (A, 16) vtable address --
   8 | void B::f0()
       [this adjustment: 0 non-virtual, -24 vcall offset offset] method: void A::f0()
   9 | void A::bar()

Virtual base offset offsets for 'B' (1 entry).
   A | -24

Thunks for 'void B::f0()' (1 entry).
   0 | this adjustment: 0 non-virtual, -24 vcall offset offset

VTable indices for 'B' (1 entries).
   0 | void B::f0()

Original map
 void D::f0() -> void B::f0()
Vtable for 'C' (10 entries).
   0 | vbase_offset (16)
   1 | offset_to_top (0)
   2 | C RTTI
       -- (C, 0) vtable address --
   3 | void C::f0()
   4 | vcall_offset (0)
   5 | vcall_offset (-16)
   6 | offset_to_top (-16)
   7 | C RTTI
       -- (A, 16) vtable address --
   8 | void C::f0()
       [this adjustment: 0 non-virtual, -24 vcall offset offset] method: void A::f0()
   9 | void A::bar()

Virtual base offset offsets for 'C' (1 entry).
   A | -24

Thunks for 'void C::f0()' (1 entry).
   0 | this adjustment: 0 non-virtual, -24 vcall offset offset

VTable indices for 'C' (1 entries).
   0 | void C::f0()
```

另一个例子：

```c
class Base
{
public:
    virtual void foo() = 0;
    virtual void bar() = 0;
};

class Der1 : public virtual Base
{
public:
    virtual void foo();
};

void Der1::foo()
{
    bar();
}

class Der2 : public virtual Base
{
public:
    virtual void bar()
    {
    }
};

class Join : public Der1, public Der2
{
public:
    // ...
};

int main()
{
    Join *p1 = new Join();
    return 0;
}
```

使用命令编译：clang -cc1 -stdlib=libc++ -fdump-record-layouts -fdump-vtable-layouts -emit-llvm ./test.cpp

```c
*** Dumping AST Record Layout
         0 | class Base
         0 |   (Base vtable pointer)
           | [sizeof=8, dsize=8, align=8,
           |  nvsize=8, nvalign=8]

*** Dumping AST Record Layout
         0 | class Der1
         0 |   class Base (primary virtual base)
         0 |     (Base vtable pointer)
           | [sizeof=8, dsize=8, align=8,
           |  nvsize=8, nvalign=8]

*** Dumping IRgen Record Layout
Record: CXXRecordDecl 0x7ff083808300 <./test.cpp:1:1, line:6:1> line:1:7 referenced class Base definition
|-DefinitionData polymorphic abstract literal has_constexpr_non_copy_move_ctor can_const_default_init
| |-DefaultConstructor exists non_trivial constexpr needs_implicit defaulted_is_constexpr
| |-CopyConstructor simple non_trivial has_const_param needs_implicit implicit_has_const_param
| |-MoveConstructor exists simple non_trivial needs_implicit
| |-CopyAssignment non_trivial has_const_param implicit_has_const_param
| |-MoveAssignment exists simple non_trivial needs_implicit
| `-Destructor simple irrelevant trivial
|-CXXRecordDecl 0x7ff083808418 <col:1, col:7> col:7 implicit class Base
|-AccessSpecDecl 0x7ff0838084a8 <line:3:1, col:7> col:1 public
|-CXXMethodDecl 0x7ff083808518 <line:4:5, col:26> col:18 foo 'void ()' virtual pure
|-CXXMethodDecl 0x7ff0838085d8 <line:5:5, col:26> col:18 referenced bar 'void ()' virtual pure
|-CXXMethodDecl 0x7ff0838086d8 <line:1:7> col:7 implicit operator= 'Base &(const Base &)' inline default noexcept-unevaluated 0x7ff0838086d8
| `-ParmVarDecl 0x7ff0838087e8 <col:7> col:7 'const Base &'
`-CXXDestructorDecl 0x7ff083808870 <col:7> col:7 implicit ~Base 'void ()' inline default trivial noexcept-unevaluated 0x7ff083808870

Layout: <CGRecordLayout
  LLVMType:%class.Base = type { i32 (...)** }
  NonVirtualBaseLLVMType:%class.Base = type { i32 (...)** }
  IsZeroInitializable:1
  BitFields:[
]>

*** Dumping IRgen Record Layout
Record: CXXRecordDecl 0x7ff083808950 <./test.cpp:8:1, line:12:1> line:8:7 class Der1 definition
|-DefinitionData polymorphic abstract can_const_default_init
| |-DefaultConstructor exists non_trivial needs_implicit
| |-CopyConstructor simple non_trivial has_const_param needs_implicit implicit_has_const_param
| |-MoveConstructor exists simple non_trivial needs_implicit
| |-CopyAssignment non_trivial has_const_param implicit_has_const_param
| |-MoveAssignment exists simple non_trivial needs_implicit
| `-Destructor simple irrelevant trivial
|-virtual public 'Base'
|-CXXRecordDecl 0x7ff083808ac0 <col:1, col:7> col:7 implicit class Der1
|-AccessSpecDecl 0x7ff083808b50 <line:10:1, col:7> col:1 public
|-CXXMethodDecl 0x7ff083808b98 <line:11:5, col:22> col:18 foo 'void ()' virtual
| `-Overrides: [ 0x7ff083808518 Base::foo 'void ()' ]
|-CXXMethodDecl 0x7ff083808c98 <line:8:7> col:7 implicit operator= 'Der1 &(const Der1 &)' inline default noexcept-unevaluated 0x7ff083808c98
| `-ParmVarDecl 0x7ff083808da8 <col:7> col:7 'const Der1 &'
`-CXXDestructorDecl 0x7ff083808e30 <col:7> col:7 implicit ~Der1 'void ()' inline default trivial noexcept-unevaluated 0x7ff083808e30

Layout: <CGRecordLayout
  LLVMType:%class.Der1 = type { %class.Base }
  NonVirtualBaseLLVMType:%class.Der1 = type { %class.Base }
  IsZeroInitializable:1
  BitFields:[
]>
Original map
Vtable for 'Base' (4 entries).
   0 | offset_to_top (0)
   1 | Base RTTI
       -- (Base, 0) vtable address --
   2 | void Base::foo() [pure]
   3 | void Base::bar() [pure]

VTable indices for 'Base' (2 entries).
   0 | void Base::foo()
   1 | void Base::bar()

Original map
 void Der1::foo() -> void Base::foo()
Vtable for 'Der1' (7 entries).
   0 | vbase_offset (0)
   1 | vcall_offset (0)
   2 | vcall_offset (0)
   3 | offset_to_top (0)
   4 | Der1 RTTI
       -- (Base, 0) vtable address --
       -- (Der1, 0) vtable address --
   5 | void Der1::foo()
   6 | void Base::bar() [pure]

Virtual base offset offsets for 'Der1' (1 entry).
   Base | -40

Thunks for 'void Der1::foo()' (1 entry).
   0 | this adjustment: 0 non-virtual, -24 vcall offset offset

VTable indices for 'Der1' (1 entries).
   0 | void Der1::foo()


*** Dumping AST Record Layout
         0 | class Der2
         0 |   class Base (primary virtual base)
         0 |     (Base vtable pointer)
           | [sizeof=8, dsize=8, align=8,
           |  nvsize=8, nvalign=8]

*** Dumping AST Record Layout
         0 | class Join
         0 |   class Der1 (primary base)
         8 |   class Der2 (base)
         0 |   class Base (virtual base)
         0 |     (Base vtable pointer)
           | [sizeof=16, dsize=16, align=8,
           |  nvsize=16, nvalign=8]

*** Dumping IRgen Record Layout
Record: CXXRecordDecl 0x7ff083836590 <./test.cpp:19:1, line:25:1> line:19:7 referenced class Der2 definition
|-DefinitionData polymorphic abstract can_const_default_init
| |-DefaultConstructor exists non_trivial
| |-CopyConstructor simple non_trivial has_const_param implicit_has_const_param
| |-MoveConstructor exists simple non_trivial needs_implicit
| |-CopyAssignment non_trivial has_const_param implicit_has_const_param
| |-MoveAssignment exists simple non_trivial needs_implicit
| `-Destructor simple irrelevant trivial
|-virtual public 'Base'
|-CXXRecordDecl 0x7ff083836700 <col:1, col:7> col:7 implicit class Der2
|-AccessSpecDecl 0x7ff083836790 <line:21:1, col:7> col:1 public
|-CXXMethodDecl 0x7ff0838367d8 <line:22:5, line:24:5> line:22:18 bar 'void ()' virtual
| |-Overrides: [ 0x7ff0838085d8 Base::bar 'void ()' ]
| `-CompoundStmt 0x7ff083836b78 <line:23:5, line:24:5>
|-CXXMethodDecl 0x7ff0838368d8 <line:19:7> col:7 implicit operator= 'Der2 &(const Der2 &)' inline default noexcept-unevaluated 0x7ff0838368d8
| `-ParmVarDecl 0x7ff0838369e8 <col:7> col:7 'const Der2 &'
|-CXXDestructorDecl 0x7ff083836a70 <col:7> col:7 implicit ~Der2 'void ()' inline default trivial noexcept-unevaluated 0x7ff083836a70
|-CXXConstructorDecl 0x7ff08300b350 <col:7> col:7 implicit used Der2 'void () throw()' inline default
| `-CompoundStmt 0x7ff08287b898 <col:7>
`-CXXConstructorDecl 0x7ff08287b718 <col:7> col:7 implicit Der2 'void (const Der2 &)' inline default noexcept-unevaluated 0x7ff08287b718
  `-ParmVarDecl 0x7ff08287b828 <col:7> col:7 'const Der2 &'

Layout: <CGRecordLayout
  LLVMType:%class.Der2 = type { %class.Base }
  NonVirtualBaseLLVMType:%class.Der2 = type { %class.Base }
  IsZeroInitializable:1
  BitFields:[
]>

*** Dumping IRgen Record Layout
Record: CXXRecordDecl 0x7ff083836b88 <./test.cpp:27:1, line:31:1> line:27:7 referenced class Join definition
|-DefinitionData polymorphic can_const_default_init
| |-DefaultConstructor exists non_trivial
| |-CopyConstructor simple non_trivial has_const_param implicit_has_const_param
| |-MoveConstructor exists simple non_trivial needs_implicit
| |-CopyAssignment non_trivial has_const_param implicit_has_const_param
| |-MoveAssignment exists simple non_trivial needs_implicit
| `-Destructor simple irrelevant trivial
|-public 'Der1'
|-public 'Der2'
|-CXXRecordDecl 0x7ff083836d40 <col:1, col:7> col:7 implicit class Join
|-AccessSpecDecl 0x7ff083836dd0 <line:29:1, col:7> col:1 public
|-CXXMethodDecl 0x7ff083836e58 <line:27:7> col:7 implicit operator= 'Join &(const Join &)' inline default noexcept-unevaluated 0x7ff083836e58
| `-ParmVarDecl 0x7ff083836f68 <col:7> col:7 'const Join &'
|-CXXDestructorDecl 0x7ff083836ff0 <col:7> col:7 implicit ~Join 'void ()' inline default trivial noexcept-unevaluated 0x7ff083836ff0
|-CXXConstructorDecl 0x7ff08300afb8 <col:7> col:7 implicit used Join 'void () throw()' inline default
| |-CXXCtorInitializer 'Base'
| | `-CXXConstructExpr 0x7ff08300b6b8 <col:7> 'Base' 'void () throw()'
| |-CXXCtorInitializer 'Der1'
| | `-CXXConstructExpr 0x7ff08287b6b8 <col:7> 'Der1' 'void () throw()'
| |-CXXCtorInitializer 'Der2'
| | `-CXXConstructExpr 0x7ff08287b8a8 <col:7> 'Der2' 'void () throw()'
| `-CompoundStmt 0x7ff08287b920 <col:7>
`-CXXConstructorDecl 0x7ff08300b090 <col:7> col:7 implicit Join 'void (const Join &)' inline default noexcept-unevaluated 0x7ff08300b090
  `-ParmVarDecl 0x7ff08300b1a8 <col:7> col:7 'const Join &'

Layout: <CGRecordLayout
  LLVMType:%class.Join = type { %class.Der1, %class.Der2 }
  NonVirtualBaseLLVMType:%class.Join = type { %class.Der1, %class.Der2 }
  IsZeroInitializable:1
  BitFields:[
]>
Original map
 void Der2::bar() -> void Base::bar()
 void Der1::foo() -> void Base::foo()
Vtable for 'Join' (14 entries).
   0 | vbase_offset (0)
   1 | vcall_offset (8)
   2 | vcall_offset (0)
   3 | offset_to_top (0)
   4 | Join RTTI
       -- (Base, 0) vtable address --
       -- (Der1, 0) vtable address --
       -- (Join, 0) vtable address --
   5 | void Der1::foo()
   6 | void Der2::bar()
       [this adjustment: 0 non-virtual, -32 vcall offset offset] method: void Base::bar()
   7 | vbase_offset (-8)
   8 | vcall_offset (0)
   9 | vcall_offset (-8)
  10 | offset_to_top (-8)
  11 | Join RTTI
       -- (Der2, 8) vtable address --
  12 | [unused] void Der1::foo()
  13 | void Der2::bar()

Virtual base offset offsets for 'Join' (1 entry).
   Base | -40


Original map
 void Der2::bar() -> void Base::bar()
 void Der1::foo() -> void Base::foo()
Construction vtable for ('Der1', 0) in 'Join' (7 entries).
   0 | vbase_offset (0)
   1 | vcall_offset (0)
   2 | vcall_offset (0)
   3 | offset_to_top (0)
   4 | Der1 RTTI
       -- (Base, 0) vtable address --
       -- (Der1, 0) vtable address --
   5 | void Der1::foo()
   6 | void Base::bar() [pure]

Original map
 void Der2::bar() -> void Base::bar()
 void Der1::foo() -> void Base::foo()
Construction vtable for ('Der2', 8) in 'Join' (13 entries).
   0 | vbase_offset (-8)
   1 | vcall_offset (0)
   2 | vcall_offset (-8)
   3 | offset_to_top (0)
   4 | Der2 RTTI
       -- (Der2, 8) vtable address --
   5 | [unused] void Base::foo() [pure]
   6 | void Der2::bar()
   7 | vcall_offset (8)
   8 | vcall_offset (0)
   9 | offset_to_top (8)
  10 | Der2 RTTI
       -- (Base, 0) vtable address --
  11 | void Base::foo() [pure]
  12 | void Der2::bar()
       [this adjustment: 0 non-virtual, -32 vcall offset offset] method: void Base::bar()

Original map
 void Der2::bar() -> void Base::bar()
 void Der1::foo() -> void Base::foo()
Vtable for 'Der2' (7 entries).
   0 | vbase_offset (0)
   1 | vcall_offset (0)
   2 | vcall_offset (0)
   3 | offset_to_top (0)
   4 | Der2 RTTI
       -- (Base, 0) vtable address --
       -- (Der2, 0) vtable address --
   5 | void Base::foo() [pure]
   6 | void Der2::bar()

Virtual base offset offsets for 'Der2' (1 entry).
   Base | -40

Thunks for 'void Der2::bar()' (1 entry).
   0 | this adjustment: 0 non-virtual, -32 vcall offset offset

VTable indices for 'Der2' (1 entries).
   1 | void Der2::bar()
```

## reference

1. [C++中虚函数、虚继承内存模型](https://zhuanlan.zhihu.com/p/41309205) 介绍了虚继承的C\++对象内存布局
2. [C.129: When designing a class hierarchy, distinguish between implementation inheritance and interface inheritance](https://www.modernescpp.com/index.php/c-core-guidelines-more-rules-to-class-hierarchies)
3. [C++ Decorate basic_iostream classes](https://stackoverflow.com/questions/6384860/c-decorate-basic-iostream-classes) 教你如何继承std::basic_streambuf和std::basic_ostream来实现自己的标准输出。
4. [【C++拾遗】 从内存布局看C++虚继承的实现原理](https://blog.csdn.net/Xiejingfa/article/details/48028491)
5. [Solving the Diamond Problem with Virtual Inheritance](https://www.cprogramming.com/tutorial/virtual_inheritance.html)
6. [What is the “dreaded diamond”?](https://isocpp.org/wiki/faq/multiple-inheritance#mi-diamond) 这是isocpp上的FAQ，该问答之后的几个问答都非常好。