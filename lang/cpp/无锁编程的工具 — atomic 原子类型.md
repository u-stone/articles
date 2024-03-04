[TOC]

# 概述

下文中引用的**N4820**是2019-06-17发布的，接近C++20标准；**N4296**是2014-11-19发布的，接近C++11标准

# 基本atomic类型

C++中的所有基本数据类型都有对应的atomic特化类型，以及声明好的类型，比如：

*   bool -- std::atomic\_bool -- std::atomic\<bool>
*   int -- std::atomic\_int -- std::atomic\<int>
*   ...

C++标准库中使用到的类型都有对应的atomic类型声明，比如：

*   intptr\_t -- std::atomic\_intptr\_t
*   size\_t -- std::atomic\_size\_t
*   int32\_t -- std::atomic\_int32\_t
*   ...

## 常用的操作方法

*   可以使用裸值初始化（std::atomic\_flag除外），不可以赋值构造，有默认构造函数。
*   is\_lock\_free() 判断是否是lock free的，因为有的类型需要使用mutex这类操作帮忙完成原子操作。并不是所有atomic类型都是lock free的。
*   store() 设置值
*   load() 取值
*   exchange() 设置值
*   compare\_exchange\_weak()
*   compare\_exchange\_strong()
*   fetch\_add()、fetch\_sub()、fetch\_and()、fetch\_or()、fetch\_xor()
*   支持operator++/--/+=/-=/&=/|=/^=操作符

一种较为特别的atomic类型是std::atomic\_flag，这个类型只有两种状态，而且初始化必须使用ATOMIC\_FLAG\_INIT，设置false状态需要用clear()函数，设置true需要使用test\_and\_set()函数。不过std::atomic\_flag类型一定是lock free的，所以这个类型也就没有is\_lock\_free方法。

# 可以构造为atomic类型的自定义类型

```c
template<class T>
struct atomic<T>;

template<class T>
struct atomic<T*>;
```

自定义类型T在满足一定条件前提下，也是可以实例化为原子类型的，这个条件就是T要满足：

1.  T是TriviallyCopyable
2.  T是CopyConstructible
3.  T是CopyAssignable

**TriviallyCopyable**这个定义要求满足下面所有条件：

1.  Copy Constructor是trivial的，或者被定义为deleted
2.  Move Contrcuctor是trivial的，或者deleted
3.  Copy Assignment Operator是trivial的，或者deleted
4.  Move Assignment Operator是trivial的，或者deleted
5.  以上4个至少有一个没有deleteed
6.  没有标记为deleted的Destrctor是trivial的

trivial在韦氏大字典中的解释是，当形容词的时候是说:

1.  lacking importance
2.  so small or unimportant as to warrant little or no attention

那么一个自定义类型T的Copy Constructor如何才能算是**trivial**的呢？需要满足以下所有条件：

1.  T没有用户自定义的Copy Constructor，也就是隐式定义implicitly-defined copy constructor，或者是被标记为default（如果一个函数被定义为default就是显示地要求编译器合成）
2.  T没有virtual成员函数
3.  T没有virtual基类
4.  T的每一个基类被选用的Copy Constructor都是trivial的
5.  T的每一个非静态的类类型成员的被选用的Copy Constructor都是trivial的

所有非联合（non-union）的类类型（class type）的trivial copy constructor就是：满足可以直接使用内存拷贝（比如说memcpy、memmove）的方式拷贝（除了padding byte之外的）内存数据就完成copy动作的类型的copy constructor。

C++中的POD（Plain Old Data）类类型就是TriviallyCopyale的，而POD简单地就是说：没有构造函数，没有析构函数，没有虚成员函数的类类型。

另外C++标准库提供了一个新的模版来判断一个类型是否是POD类型(std::is\_pod\<T>::value)：

```c
#include <iostream>
#include <type_traits>
 
struct A {
    int m;
};
 
struct B {
    int m1;
private:
    int m2;
};
 
struct C {
    virtual void foo();
};
 
int main()
{
    std::cout << std::boolalpha;
    std::cout << std::is_pod<A>::value << '\n';
    std::cout << std::is_pod<B>::value << '\n';
    std::cout << std::is_pod<C>::value << '\n';
}

输出:  
true
false
false
```

这个模版是在C++11中引入的，但是在C++20中就会被标记为弃用。C++11引入了了一系列类型判断的支持，具体还有哪些类型判断可以参考 [Type support (basic types, RTTI, type traits)](https://en.cppreference.com/w/cpp/types)。

## 什么是aggregates类型

*需要注意的是，C++标准对于aggregates和POD的定义是在不断变化的，比如新的标准中加入了扩展，使得定义范围更广了*

要理解POD类型，需要先看以下aggregates的含义。

aggregates 在韦氏大字典中的含义

*   adjective. *formed by adding together two or more amounts*
*   verb. *to join or combine into a single group; to equal a specified number or amount*
*   noun. *a total amount*

中文一般翻译为“聚合”

在C++标准中，Aggregates class定义：

*   An aggregate is an array or a class (Clause 9) with no user-provided constructors (12.1), no private or protected non-static data members (Clause 11), no base classes (Clause 10), and no virtual functions (10.3).

*from open-std.org N4296《Working Draft, Standard for Programming Language C++》*

```c
An aggregate is an array or a class (Clause 11) with
— no user-declared or inherited constructors (11.3.4),
— no private or protected direct non-static data members (11.8), 
— no virtual functions (11.6.2), and
— no virtual, private, or protected base classes (11.6.1).
[Note: Aggregate initialization does not allow accessing protected and 
private base class’ members or constructors. —end note]
```

*from open-std.org N4820《Working Draft, Standard for Programming Language C++》*

对比标准的变化可以看出，约束条件是放宽了。C++11是要求不能有基类，但是C++20是说不能有private/protected基类。

这里需要注意的是，aggregates类类型不能有用户定义的构造函数，但是可以有用户定义的copy assignment operator和destructor。C++11标准里面定义不能有in-class member initializers，但是C++14开始允许了。

```c
struct A
{
  int a = 3;
  int b = 3;
};
```

有了aggregates的定义，就可以看什么是POD类型了。

## POD类型

自C++11标准开始POD的定义有了比较大的变化，POD的本意是为了：

*   支持静态初始化
*   想要在C++中编译POD类型得到的内存布局，与C的struct编译后的内存布局一致

所以，C++11开始POD总体上分裂为trivial class和standard-layout class两个定义。标准里面也很少提到POD的概念，一般提到POD也都是说是一种同时满足trivial和standard-layout的class。具体如下

*   A POD struct is a non-union class that is both a **trivial class** and a **standard-layout class**, and has no non-static data members of type non-POD struct, non-POD union (or array of such types). Similarly, a POD union is a union that is both a trivial class and a standard-layout class, and has no non-static data members of type non-POD struct, non-POD union (or array of such types). A POD class is a class that is either a POD struct or a POD union.

```c
[ Example:
  struct N { // neither trivial nor standard-layout
    int i;
    int j;
    virtual ~N(); 
  };
  
  struct T {  // trivial but not standard-layout
    int i;
  private:
    int j;
  };
  
  struct SL {  // standard-layout but not trivial
    int i;
    int j;
    ~SL();
  };
  
  struct POD {  // both trivial and standard-layout
    int i;
    int j; 
  };
  
— end example ]
```

*from open-std.org N4296《Working Draft, Standard for Programming Language C++》*

*   A POD class is a class that is both a **trivial class** and a **standard-layout class**, and has no non-static data members of type non-POD class (or array thereof). A POD type is a scalar type, a POD class, an array of such a type, or a cv-qualified version of one of these types.\
    *from open-std.org N4820《Working Draft, Standard for Programming Language C++》*

C++中的POD class（C++中的struct，union都是类类型）本不同于C中的POD struct，因为C中没有构造函数，没有成员函数，没有静态变量。但是可以说C++中的POD类型是最接近C中struct的类型。

### trivial class

先看以下标准中关于trivial class的定义，不过它是trivially copyable的一个子集

```c
A trivially copyable class is a class that:  
- has no non-trivial copy constructors (12.8),
- has no non-trivial move constructors (12.8),
- has no non-trivial copy assignment operators (13.5.3, 12.8),
- has no non-trivial move assignment operators (13.5.3, 12.8), and
- has a trivial destructor (12.4).  
A trivial class is a class that has a default constructor (12.1), 
has no non-trivial default constructors, and is trivially copyable. 
[ Note: In particular, a trivially copyable or 
trivial class does not have virtual functions or virtual base classes. — end note ] 
```

*from open-std.org N4296《Working Draft, Standard for Programming Language C++》*

光看定义会觉得很绕，因为这个定义使用了递归的方式。不过定义中给的Note是非常直观的。

在C++11中，trivial class就是说一个类没有虚函数，*递归地*规定其基类也没有虚函数。这样就可以满足，用一个内存拷贝函数(比如memcpy，memmove)直接拷贝一份trivial class的对象在内存中的数据，然后使用这段内存覆盖到另一个此类对象上，将得到一个一摸一样的对象。

```c
// empty classes are trivial
struct Trivial1 {};

// all special members are implicit
struct Trivial2 {
    int x;
};

struct Trivial3 : Trivial2 { // base class is trivial
    Trivial3() = default; // not a user-provided ctor
    int y;
};

struct Trivial4 {
public:
    int a;
private: // no restrictions on access modifiers
    int b;
};

struct Trivial5 {
    Trivial1 a;
    Trivial2 b;
    Trivial3 c;
    Trivial4 d;
};

struct Trivial6 {
    Trivial2 a[23];
};

struct Trivial7 {
    Trivial6 c;
    void f(); // it's okay to have non-virtual functions
};

struct Trivial8 {
     int x;
     static NonTrivial1 y; // no restrictions on static members
};

struct Trivial9 {
     Trivial9() = default; // not user-provided
      // a regular constructor is okay because we still have default ctor
     Trivial9(int x) : x(x) {};
     int x;
};

struct NonTrivial1 : Trivial3 {
    virtual void f(); // virtual members make non-trivial ctors
};

struct NonTrivial2 {
    NonTrivial2() : z(42) {} // user-provided ctor
    int z;
};

struct NonTrivial3 {
    NonTrivial3(); // user-provided ctor
    int w;
};
NonTrivial3::NonTrivial3() = default; // defaulted but not on first declaration
                                      // still counts as user-provided
struct NonTrivial5 {
    virtual ~NonTrivial5(); // virtual destructors are not trivial
};
```

较新的标准中定义有所变化

```c
A trivially copyable class is a class:
— where each copy constructor, move constructor, copy assignment operator, 
  and move assignment operator (11.3.4.2, 11.3.5) is either deleted or trivial,
— that has at least one non-deleted copy constructor, move constructor, 
  copy assignment operator, or move assignment operator, and
— that has a trivial, non-deleted destructor (11.3.6).
A trivial class is a class that is trivially copyable and 
has one or more default constructors (11.3.4.1), all of which are either trivial or
deleted and at least one of which is not deleted. 
[Note: In particular, a trivially copyable or trivial class does not 
have virtual functions or virtual base classes.—end note]
```

*from open-std.org N4820《Working Draft, Standard for Programming Language C++》*

### standard-layout class

standard-layout class是在C++与其他语言通信的时候十分有用，因为这种类对象的内存布局与C的struct/union结构相同。C++11以前规定类的non-static成员变量必须都是public的，现在规定放宽到只要是同一种访问权限(has the same access control)即可，标准中关于standard-layout class的定义如下

```c
A class S is a standard-layout class if it:
- has no non-static data members of type non-standard-layout class (or array of such types) or reference,
- has no virtual functions (10.3) and no virtual base classes (10.1),
- has the same access control (Clause 11) for all non-static data members,
- has no non-standard-layout base classes,
- has at most one base class subobject of any given type,
- has all non-static data members and bit-fields in the class and 
  its base classes first declared in the same class, and
- has no element of the set M(S) of types (defined below) as a base class.

M(X) is defined as follows:  

- If X is a non-union class type, the set M(X) is empty if X has no 
(possibly inherited (Clause 10)) non-static data members; 
otherwise, it consists of the type of the first non-static data member of X (where said
member may be an anonymous union), X0, and the elements of M(X0).
- If X is a union type, the set M(X) is the union of all M(Ui) and the set containing all Ui, 
where each Ui is the type of the ith non-static data member of X.
- If X is a non-class type, the set M(X) is empty.  
[Note: M(X) is the set of the types of all non-base-class subobjects that 
are guaranteed in a standard-layout class to be at a zero offset in X. —end note]
```

*from open-std.org N4296《Working Draft, Standard for Programming Language C++》*

```c
A class S is a standard-layout class if it:
— has no non-static data members of type non-standard-layout class (or array of such types) or reference, 
— has no virtual functions (11.6.2) and no virtual base classes (11.6.1),
— has the same access control (11.8) for all non-static data members,
— has no non-standard-layout base classes,
— has at most one base class subobject of any given type,
— has all non-static data members and bit-fields in the class and its base classes 
first declared in the same class, and
— has no element of the set M(S) of types as a base class, where for any type X, 

M(X) is defined as follows.

[Note: M(X) is the set of the types of all non-base-class subobjects that 
may be at a zero offset in X. —end note]
    — If X is a non-union class type with no (possibly inherited (11.6)) 
       non-static data members, the set M(X) is empty.
    — If X is a non-union class type with a non-static data member of type X0 
       that is either of zero size or is the first non-static data member of X (where said
       member may be an anonymous union), the set M(X) consists of X0 and the elements of M(X0).
    — If X is a union type, the set M(X) is the union of all M(Ui) and the set containing all Ui, 
       where each Ui is the type of the ith non-static data member of X.
    — If X is an array type with element type Xe, the set M(X) consists of Xe and the elements of M(Xe).
    — If X is a non-class, non-array type, the set M(X) is empty.

[Example:
struct B {int i; }; // standard-layout class
struct C : B {}; // standard-layout class
struct D : C {}; // standard-layout class
struct E : D { char : 4; }; // not a standard-layout class

struct Q {};
struct S : Q { }; 
struct T : Q { }; 
struct U : S,T{ }; // not a standard-layout class
—end example]
```

*from open-std.org N4820《Working Draft, Standard for Programming Language C++》*

```c
// empty classes have standard-layout
struct StandardLayout1 {};

struct StandardLayout2 {
    int x;
};

struct StandardLayout3 {
private: // both are private, so it's ok
    int x;
    int y;
};

struct StandardLayout4 : StandardLayout1 {
    int x;
    int y;

    void f(); // perfectly fine to have non-virtual functions
};

struct StandardLayout5 : StandardLayout1 {
    int x;
    StandardLayout1 y; // can have members of base type if they're not the first
};

struct StandardLayout6 : StandardLayout1, StandardLayout5 {
    // can use multiple inheritance as long only
    // one class in the hierarchy has non-static data members
};

struct StandardLayout7 {
    int x;
    int y;
    StandardLayout7(int x, int y) : x(x), y(y) {} // user-provided ctors are ok
};

struct StandardLayout8 {
public:
    StandardLayout8(int x) : x(x) {} // user-provided ctors are ok
// ok to have non-static data members and other members with different access
private:
    int x;
};

struct StandardLayout9 {
    int x;
    static NonStandardLayout1 y; // no restrictions on static members
};

struct NonStandardLayout1 {
    virtual f(); // cannot have virtual functions
};

struct NonStandardLayout2 {
    NonStandardLayout1 X; // has non-standard-layout member
};

struct NonStandardLayout3 : StandardLayout1 {
    StandardLayout1 x; // first member cannot be of the same type as base
};

struct NonStandardLayout4 : StandardLayout3 {
    int z; // more than one class has non-static data members
};

struct NonStandardLayout5 : NonStandardLayout3 {}; // has a non-standard-layout base class
```

上面提到这么多，是要搞清楚什么样的自定义类型可以做成原子类型。所以现在言归正传。其实最实用的说法就是没有虚函数，没有虚基类，访问控制只有一种的class就是可以做成原子类型的。而且C++标准库提供了几个方法：

*   std::is\_trivially\_copyable\<T>::value
*   std::is\_copy\_constructible\<T>::value
*   std::is\_move\_constructible\<T>::value
*   std::is\_copy\_assignable\<T>::value
*   std::is\_move\_assignable\<T>::value

*from <https://en.cppreference.com/w/cpp/atomic/atomic>*

只要以上条件中有一个是false，那么这个类型T就是不可以做成atomic的。

对于指针类型的自定义类T\*

***

# 参考

1.  [std::memory\_order](https://en.cppreference.com/w/cpp/atomic/memory_order) 这绝对是考验英语水平的一篇文章
2.  [Atomic operations library](https://en.cppreference.com/w/cpp/atomic)
3.  [《C++ Concurrency In Action》](https://book.douban.com/subject/4130141/)
4.  stack overflow上关于C++11/14/20对POD类型变化讲的十分详细的问题[What are Aggregates and PODs and how/why are they special?](https://stackoverflow.com/questions/4178175/what-are-aggregates-and-pods-and-how-why-are-they-special/4178176#4178176)
5.  发现了stack overflow上一个C++的资源帖子：[about C++ FAQ](https://stackoverflow.com/tags/c%2b%2b-faq/info)；关于C++中的惯用法有个不错的资源：[More C++ Idioms](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms)
6.  C++标准最终版本是收费的，但是一般来说我们看草稿就可以了，而且这个是免费的，具体的pdf文件可以在[C++ Standards Committee Papers](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/)里面某些年份搜索关键字“Working Draft, Standard for Programming Language C++”找到最新的草稿。
7.  即使看了函数说明也还是不理解的一对原子操作 [std::atomic | compare\_exchange\_weak vs. compare\_exchange\_strong](https://stackoverflow.com/questions/4944771/stdatomic-compare-exchange-weak-vs-compare-exchange-strong)  on stackoverflow
8.  1995年Compaq康柏公司（后于2002年被惠普收购）的一个技术报告：WRL Research Report 95/7 [Shared Memory Consistency Models\:A Tutorial](https://www.hpl.hp.com/techreports/Compaq-DEC/WRL-95-7.pdf) SEPTEMBER 1995
9.  一本Linux kernel的电子书，该书会经常不定期更新的 [Is Parallel Programming Hard, And, If So, What Can You Do About It?](https://mirrors.edge.kernel.org/pub/linux/kernel/people/paulmck/perfbook/perfbook.html)   。中文版的发行于2017年 [深入理解并行编程](https://book.douban.com/subject/27078711/)，看不出原版是哪一版本，不过被人吐槽翻译的不好。国内网速太差，附上电子书百度网盘的[链接](https://pan.baidu.com/s/1lJs6Ew4t5Lmz_KqzgdZVxw) 提取码: 2c8x。 如果是要看7中提到的问题，那么StackOverflow的答主建议阅读第12章（估计是现在的15章 Advanced Synchronization: Memory Ordering）以及附录C。
10. 如果在一个struct里面添加一个0bit的filed会有什么影响，参考MSDN上[C++ Bit Fields](https://docs.microsoft.com/en-us/cpp/cpp/cpp-bit-fields?redirectedfrom=MSDN\&view=vs-2019)，the answer is "Force alignment to next boundary."

