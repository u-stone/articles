*分析的源码对应的是 `Xcode 12.0.1` 中 `llvm` 关于 `libc++` 的实现。`llvm` 源码[地址](https://github.com/llvm/llvm-project)，及 `llvm` [下载地址汇总](https://releases.llvm.org/download.html)。`libc++` 的[文档](https://releases.llvm.org/16.0.0/projects/libcxx/docs/index.html)*

[toc]


*代码分析的思路是，按照 [cppreference.com](https://en.cppreference.com/) 网站上列出的智能指针接口顺序，依据 llvm 中的源代码，借助 copilot 的帮助，分类别、按序分析。*


# 支持的操作

参考 [std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr) 中的介绍：

成员函数：

- constructor
- destructor
- operator=， C++20及之前不支持
- release
- reset
- swap
- get
- get_deleter
- operator bool
- operator*
- operator->
- operator[]

非成员函数：

- make_unique
- operator==,!=, <, <=, >, >=, <=>
- operator<<(std::unique_ptr) (since C++20)
- std::swap(std::unique_ptr)
- `std::hash<std::unique_ptr>`

# 具体分析

先看一下创建过程

## 新建对象

### make_unique
下面是源码：

```C++

//template<class T, class... Args> unique_ptr<T> make_unique(Args&&... args);     // C++14
//template<class T>                unique_ptr<T> make_unique(size_t n);           // C++14
//template<class T, class... Args> unspecified   make_unique(Args&&...) = delete; // C++14, T == U[N]

#if _LIBCPP_STD_VER > 11

template<class _Tp>
struct __unique_if
{
    typedef unique_ptr<_Tp> __unique_single;
};

template<class _Tp>
struct __unique_if<_Tp[]>
{
    typedef unique_ptr<_Tp[]> __unique_array_unknown_bound;
};

template<class _Tp, size_t _Np>
struct __unique_if<_Tp[_Np]>
{
    typedef void __unique_array_known_bound;
};

template<class _Tp, class... _Args>
inline _LIBCPP_INLINE_VISIBILITY
typename __unique_if<_Tp>::__unique_single
make_unique(_Args&&... __args)
{
    return unique_ptr<_Tp>(new _Tp(_VSTD::forward<_Args>(__args)...));
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
typename __unique_if<_Tp>::__unique_array_unknown_bound
make_unique(size_t __n)
{
    typedef typename remove_extent<_Tp>::type _Up;
    return unique_ptr<_Tp>(new _Up[__n]());
}

template<class _Tp, class... _Args>
    typename __unique_if<_Tp>::__unique_array_known_bound
    make_unique(_Args&&...) = delete;

#endif  // _LIBCPP_STD_VER > 11
```

正如上面提到的 `make_unique` 是从 C++14 开始支持的。

可以看到一共定义了两个版本：一个创建单元素对象，一个创建一个对象数组。这两个方法支持下面这样创建对象

```C++
class Base {
public:
    Base() = default;
    Base(int v):a(v), b(2) {}
private:
    int a;
    int b;
};

int main() {
    std::unique_ptr<Base> usp = std::make_unique<Base>(); // 单对象版本
    std::unique_ptr<Base[]> usp_ary = std::make_unique<Base[]>(5); // 数组版本，这里创建5个默认Base对象。
}
```

从实现来看，`make_unique` 
- 创建单个对象的时候支持传入参数，调用相应的构造函数。对应代码 `return unique_ptr<_Tp>(new _Tp(_VSTD::forward<_Args>(__args)...));`
- 而数组版本，无法支持传入参数，只能调用默认构造函数。对应代码 `return unique_ptr<_Tp>(new _Up[__n]());`

这里说明一下数组版本为什么不提供传参数。参考 [Is it possible to pass arguments to std::make_unique() when allocating an array?](https://stackoverflow.com/questions/64557294/is-it-possible-to-pass-arguments-to-stdmake-unique-when-allocating-an-array)：

简言之，提供一个带有参数的版本没有什么用处。比如你可以这样提供一个数组版本：`std::unique_ptr<Base[]> usp_ary2(new Base[3]{200, 300, 400});`，而不需要使用 `make_unique` 函数。当然这**也**没有什么用处。

同时明确删除掉一个数组版本指定数组长度的声明。这样就会导致下面的代码会编译错误：

```C++
// error: Call to deleted function 'make_unique'
std::unique_ptr<Base[3]> usp_ary = std::make_unique<Base[3]>(4); 
```

>
> `make_unique` 的来龙去脉可以参考C++标准化的这个文档： [make_unique](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3588.txt)，这个文档在 [Why is there a make_unique? Why not just overload the unique_ptr constructor?](https://devblogs.microsoft.com/oldnewthing/20221019-00/?p=107300) 中有提及。
>

在 memory 文件中注释掉的代码中，可以看到一个简化版本的类定义：

```C++
template <class T>
struct default_delete
{
    constexpr default_delete() noexcept = default;
    template <class U> default_delete(const default_delete<U>&) noexcept;

    void operator()(T*) const noexcept;
};

template <class T>
struct default_delete<T[]>
{
    constexpr default_delete() noexcept = default;
    void operator()(T*) const noexcept;
    template <class U> void operator()(U*) const = delete;
};

template <class T, class D = default_delete<T>>
class unique_ptr
{
public:
    typedef see below pointer;
    typedef T element_type;
    typedef D deleter_type;

    // constructors
    constexpr unique_ptr() noexcept;
    explicit unique_ptr(pointer p) noexcept;
    unique_ptr(pointer p, see below d1) noexcept;
    unique_ptr(pointer p, see below d2) noexcept;
    unique_ptr(unique_ptr&& u) noexcept;
    unique_ptr(nullptr_t) noexcept : unique_ptr() { }
    template <class U, class E>
        unique_ptr(unique_ptr<U, E>&& u) noexcept;
    template <class U>
        unique_ptr(auto_ptr<U>&& u) noexcept;       // removed in C++17

    // destructor
    ~unique_ptr();

    // assignment
    unique_ptr& operator=(unique_ptr&& u) noexcept;
    template <class U, class E> unique_ptr& operator=(unique_ptr<U, E>&& u) noexcept;
    unique_ptr& operator=(nullptr_t) noexcept;

    // observers
    typename add_lvalue_reference<T>::type operator*() const;
    pointer operator->() const noexcept;
    pointer get() const noexcept;
    deleter_type& get_deleter() noexcept;
    const deleter_type& get_deleter() const noexcept;
    explicit operator bool() const noexcept;

    // modifiers
    pointer release() noexcept;
    void reset(pointer p = pointer()) noexcept;
    void swap(unique_ptr& u) noexcept;
};

template <class T, class D>
class unique_ptr<T[], D>
{
public:
    typedef implementation-defined pointer;
    typedef T element_type;
    typedef D deleter_type;

    // constructors
    constexpr unique_ptr() noexcept;
    explicit unique_ptr(pointer p) noexcept;
    unique_ptr(pointer p, see below d) noexcept;
    unique_ptr(pointer p, see below d) noexcept;
    unique_ptr(unique_ptr&& u) noexcept;
    unique_ptr(nullptr_t) noexcept : unique_ptr() { }

    // destructor
    ~unique_ptr();

    // assignment
    unique_ptr& operator=(unique_ptr&& u) noexcept;
    unique_ptr& operator=(nullptr_t) noexcept;

    // observers
    T& operator[](size_t i) const;
    pointer get() const noexcept;
    deleter_type& get_deleter() noexcept;
    const deleter_type& get_deleter() const noexcept;
    explicit operator bool() const noexcept;

    // modifiers
    pointer release() noexcept;
    void reset(pointer p = pointer()) noexcept;
    void reset(nullptr_t) noexcept;
    template <class U> void reset(U) = delete;
    void swap(unique_ptr& u) noexcept;
};
```

我们接着分析代码。

### constructor & destructor

具体到构造函数与析构函数，`std::unique_ptr` 支持：

```C++
  // 第一组 构造函数
  template <bool _Dummy = true,
            class = _EnableIfDeleterDefaultConstructible<_Dummy> >
  _LIBCPP_INLINE_VISIBILITY
  _LIBCPP_CONSTEXPR unique_ptr() _NOEXCEPT : __ptr_(pointer(), __default_init_tag()) {}

  template <bool _Dummy = true,
            class = _EnableIfDeleterDefaultConstructible<_Dummy> >
  _LIBCPP_INLINE_VISIBILITY
  _LIBCPP_CONSTEXPR unique_ptr(nullptr_t) _NOEXCEPT : __ptr_(pointer(), __default_init_tag()) {}

  template <bool _Dummy = true,
            class = _EnableIfDeleterDefaultConstructible<_Dummy> >
  _LIBCPP_INLINE_VISIBILITY
  explicit unique_ptr(pointer __p) _NOEXCEPT : __ptr_(__p, __default_init_tag()) {}



  // 第二组 构造函数
  // 对应_Deleter为 左值
  template <bool _Dummy = true,
            class = _EnableIfDeleterConstructible<_LValRefType<_Dummy> > >
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(pointer __p, _LValRefType<_Dummy> __d) _NOEXCEPT
      : __ptr_(__p, __d) {}

  // 对应_Deleter为 右值
  template <bool _Dummy = true,
            class = _EnableIfDeleterConstructible<_GoodRValRefType<_Dummy> > >
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(pointer __p, _GoodRValRefType<_Dummy> __d) _NOEXCEPT
      : __ptr_(__p, _VSTD::move(__d)) {
    static_assert(!is_reference<deleter_type>::value,
                  "rvalue deleter bound to reference");
  }
  // 对应_Deleter为 右值引用。 不允许把一个右值引用（const和非const）初始化为unique_ptr对象。
  template <bool _Dummy = true,
            class = _EnableIfDeleterConstructible<_BadRValRefType<_Dummy> > >
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(pointer __p, _BadRValRefType<_Dummy> __d) = delete;



  // 以下是各种形式的移动构造函数
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(unique_ptr&& __u) _NOEXCEPT
      : __ptr_(__u.release(), _VSTD::forward<deleter_type>(__u.get_deleter())) {
  }

  template <class _Up, class _Ep,
      class = _EnableIfMoveConvertible<unique_ptr<_Up, _Ep>, _Up>,
      class = _EnableIfDeleterConvertible<_Ep>
  >
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(unique_ptr<_Up, _Ep>&& __u) _NOEXCEPT
      : __ptr_(__u.release(), _VSTD::forward<_Ep>(__u.get_deleter())) {}
  // 通过 auto_ptr 构造，不过已经从 C++17 中删除
#if _LIBCPP_STD_VER <= 14 || defined(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
  template <class _Up>
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(auto_ptr<_Up>&& __p,
             typename enable_if<is_convertible<_Up*, _Tp*>::value &&
                                    is_same<_Dp, default_delete<_Tp> >::value,
                                __nat>::type = __nat()) _NOEXCEPT
      : __ptr_(__p.release(), __default_init_tag()) {}
#endif



  // 各种形式的移动赋值操作符
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr& operator=(unique_ptr&& __u) _NOEXCEPT {
    reset(__u.release());
    __ptr_.second() = _VSTD::forward<deleter_type>(__u.get_deleter());
    return *this;
  }

  template <class _Up, class _Ep,
      class = _EnableIfMoveConvertible<unique_ptr<_Up, _Ep>, _Up>,
      class = _EnableIfDeleterAssignable<_Ep>
  >
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr& operator=(unique_ptr<_Up, _Ep>&& __u) _NOEXCEPT {
    reset(__u.release());
    __ptr_.second() = _VSTD::forward<_Ep>(__u.get_deleter());
    return *this;
  }

  // 通过 auto_ptr 构造，不过已经从 C++17 中删除
#if _LIBCPP_STD_VER <= 14 || defined(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
  template <class _Up>
  _LIBCPP_INLINE_VISIBILITY
      typename enable_if<is_convertible<_Up*, _Tp*>::value &&
                             is_same<_Dp, default_delete<_Tp> >::value,
                         unique_ptr&>::type
      operator=(auto_ptr<_Up> __p) {
    reset(__p.release());
    return *this;
  }
#endif


  // 声明删除拷贝构造函数和赋值操作符
#ifdef _LIBCPP_CXX03_LANG
  unique_ptr(unique_ptr const&) = delete;
  unique_ptr& operator=(unique_ptr const&) = delete;
#endif

  // 析构函数
  _LIBCPP_INLINE_VISIBILITY
  ~unique_ptr() { reset(); }

  // 需要注意这个赋值操作符，仅支持空指针赋值。
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr& operator=(nullptr_t) _NOEXCEPT {
    reset();
    return *this;
  }
```

可以看到构造函数还是挺多的。有：

- 构造空对象的
- 通过 `nullptr` 构造空对象的
- 通过派生类，或者说可以转换为当前对象的类型 `unique_ptr` 构造一个对象
- 通过 `std::auto_ptr` 来构造，不过已经从C++17中删除了
- 移动构造函数，也有三个版本
  - 基于另一个同类型 `unique_ptr` 来构造
  - 基于派生类对象构造
  - 基于一个派生类的 `auto_ptr` 来构造。

以及被声明为 `= delete` 的两个拷贝构造函数和拷贝赋值操作符号。这个达到了 `unique_ptr` 只能被移动无法被复制的目的。

其中构造函数模版中使用的一些模版工具：

```C++
using _EnableIfDeleterDefaultConstructible _LIBCPP_NODEBUG_TYPE  =
    typename enable_if<is_default_constructible<_Deleter>::value &&
                        !is_pointer<_Deleter>::value>::type;

template <class _ArgType>
using _EnableIfDeleterConstructible _LIBCPP_NODEBUG_TYPE  =
    typename enable_if<is_constructible<deleter_type, _ArgType>::value>::type;

// unique_ptr 中定义的两个模版工具，对应左值和右值：
template <bool _Dummy>
using _LValRefType _LIBCPP_NODEBUG_TYPE =
    typename __dependent_type<_DeleterSFINAE, _Dummy>::__lval_ref_type;

template <bool _Dummy>
using _GoodRValRefType _LIBCPP_NODEBUG_TYPE =
    typename __dependent_type<_DeleterSFINAE, _Dummy>::__good_rval_ref_type;

template <bool _Dummy>
using _BadRValRefType _LIBCPP_NODEBUG_TYPE =
    typename __dependent_type<_DeleterSFINAE, _Dummy>::__bad_rval_ref_type;

// C++ 标准的基础模版工具，可以在
// /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1/type_traits 
// 中找到
template <bool, class _Tp = void> struct _LIBCPP_TEMPLATE_VIS enable_if {};
template <class _Tp> struct _LIBCPP_TEMPLATE_VIS enable_if<true, _Tp> {typedef _Tp type;};
 
#if _LIBCPP_STD_VER > 11
template <bool _Bp, class _Tp = void> using enable_if_t = typename enable_if<_Bp, _Tp>::type;
#endif

template <class _Tp, bool>
struct _LIBCPP_TEMPLATE_VIS __dependent_type : public _Tp {};
```

Q：_EnableIfDeleterDefaultConstructible的含义
A：copilot问答，得到下面的解释：

>
> 在 LLVM 的 libc++ 实现中，_EnableIfDeleterDefaultConstructible 是一个类型别名，它使用 std::enable_if 来进行条件编译。这个别名的作用是在编译时检查 _Deleter 是否可以默认构造。
> 
> std::enable_if 是一个模板元编程技术，它可以在编译时根据条件选择类型。在这个例子中，如果 _Deleter 可以默认构造（即 std::is_default_constructible<_Deleter>::value 为 true）并且 _Deleter 不是指针类型（即 !std::is_pointer<_Deleter>::value 为 true），那么 _EnableIfDeleterDefaultConstructible 就是 void 类型，否则这个类型别名不存在。
> 
> 这种技术常常用于函数模板的重载，通过使某些重载在满足特定条件时不存在，从而在编译时选择正确的函数版本。在这个例子中，_EnableIfDeleterDefaultConstructible 可能用于选择适当的 std::unique_ptr 构造函数或成员函数。
> 

Q：请解释一下C++模版编程中SFINAE（替换失败并非错误）是什么概念？
A：copilot 回答如下(没有问题的)
> SFINAE 是 C++ 模板编程中的一个重要概念，全称为 "Substitution Failure Is Not An Error"，中文可以翻译为 "替换失败并非错误"。
>
> SFINAE 描述的是一种情况：在模板实例化过程中，如果某个模板参数的替换会导致语法错误或者产生不存在的类型，那么这个错误会被忽略，而不是导致编译失败。编译器会继续尝试其他的模板，直到找到一个可以成功实例化的模板。
>
> SFINAE 常常与 std::enable_if 结合使用，来控制模板的选择。通过 std::enable_if，我们可以在编译时根据某些条件来启用或禁用某个模板。如果 std::enable_if 的条件不满足，那么对应的模板就会在编译时被移除，这就是 SFINAE 的效果。
>
> 例如，我们可以使用 SFINAE 来创建只接受整数类型参数的函数模板，如果尝试用非整数类型参数来调用这个函数，编译器就会忽略这个模板，而去寻找其他的重载版本或者产生编译错误。
>

关于具体的模版设施不再深入分析，这个是模版编程中的一部分。

**析构函数：**

- ~unique_ptr

```C++
  _LIBCPP_INLINE_VISIBILITY
  ~unique_ptr() { reset(); }
```

## 成员函数

- release

```C++
  _LIBCPP_INLINE_VISIBILITY
  pointer release() _NOEXCEPT {
    pointer __t = __ptr_.first();
    __ptr_.first() = pointer();
    return __t;
  }
```

这里的 `__ptr__` 原型是： `__compressed_pair<pointer, deleter_type> __ptr_;`，`first()` 中对应的就是 `unique_ptr` 指针保存的对象。

- reset

```C++
  _LIBCPP_INLINE_VISIBILITY
  void reset(pointer __p = pointer()) _NOEXCEPT {
    pointer __tmp = __ptr_.first();
    __ptr_.first() = __p;
    if (__tmp)
      __ptr_.second()(__tmp);
  }
```

先把老的指针所指向的值保存起来 `__tmp`，然后将 `__p` 的值传给当前对象。如果老的指针非空，那么就调用 `Deleter` 删除它。

- swap

```C++
  _LIBCPP_INLINE_VISIBILITY
  void swap(unique_ptr& __u) _NOEXCEPT {
    __ptr_.swap(__u.__ptr_);
  }
```

`unique_ptr` 的 `swap` 的实现最终调用了 `__compressed_pair` 的 `swap`：

```C++
  _LIBCPP_INLINE_VISIBILITY
  void swap(__compressed_pair& __x)
    _NOEXCEPT_(__is_nothrow_swappable<_T1>::value &&
               __is_nothrow_swappable<_T2>::value)
  {
    using std::swap;
    swap(first(), __x.first());
    swap(second(), __x.second());
  }
```

后者最终调用了 `std::swap` 的实现。

- get

```C++
  _LIBCPP_INLINE_VISIBILITY
  pointer get() const _NOEXCEPT {
    return __ptr_.first();
  }
```
最终调用了 `__compressed_pair` 方法

- get_deleter

```C++
  _LIBCPP_INLINE_VISIBILITY
  deleter_type& get_deleter() _NOEXCEPT {
    return __ptr_.second();
  }
```
最终调用了 `__compressed_pair` 方法

- operator bool()

```C++
  _LIBCPP_INLINE_VISIBILITY
  _LIBCPP_EXPLICIT operator bool() const _NOEXCEPT {
    return __ptr_.first() != nullptr;
  }
```

只是把保存的指针与 `nullptr` 进行比较。

- 操作符重载

```C++
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr& operator=(nullptr_t) _NOEXCEPT {
    reset();
    return *this;
  }

  _LIBCPP_INLINE_VISIBILITY
  typename add_lvalue_reference<_Tp>::type
  operator*() const {
    return *__ptr_.first();
  }
  _LIBCPP_INLINE_VISIBILITY
  pointer operator->() const _NOEXCEPT {
    return __ptr_.first();
  }

  // 数组版本独有的
  // 可以看出，这里并没有做下标范围检查
  _LIBCPP_INLINE_VISIBILITY
  typename add_lvalue_reference<_Tp>::type
  operator[](size_t __i) const {
    return __ptr_.first()[__i];
  }
```

代码很简单，注意一下各自的返回类型就可以了。



## 非成员函数

- swap

```C++
template <class _Tp, class _Dp>
inline _LIBCPP_INLINE_VISIBILITY
typename enable_if<
    __is_swappable<_Dp>::value,
    void
>::type
swap(unique_ptr<_Tp, _Dp>& __x, unique_ptr<_Tp, _Dp>& __y) _NOEXCEPT {__x.swap(__y);}
```

这里最终是调用了 `unique_ptr` 的 `swap` 成员函数。参考前面的介绍，最终是调用到了 `std::swap`。

- 比较操作符

实现比较简单。

重点关注一下，实际上只是定义了 `operator <, ==`， 之后 `operator !=, <=, >, >=` 都是对前二者的复用。

```C++
// == 真正的定义
template <class _T1, class _D1, class _T2, class _D2>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator==(const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) {return __x.get() == __y.get();}

// 复用 operator==
template <class _T1, class _D1, class _T2, class _D2>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) {return !(__x == __y);}

// < 真正的定义
template <class _T1, class _D1, class _T2, class _D2>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator< (const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y)
{
    typedef typename unique_ptr<_T1, _D1>::pointer _P1;
    typedef typename unique_ptr<_T2, _D2>::pointer _P2;
    typedef typename common_type<_P1, _P2>::type _Vp;
    return less<_Vp>()(__x.get(), __y.get());
}

// 复用 operator <，交换参数，得到 operator >
template <class _T1, class _D1, class _T2, class _D2>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator> (const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) {return __y < __x;}

// 复用
template <class _T1, class _D1, class _T2, class _D2>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<=(const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) {return !(__y < __x);}
// 复用
template <class _T1, class _D1, class _T2, class _D2>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>=(const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) {return !(__x < __y);}
// 真正的定义，特殊类型 nullptr_t
template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator==(const unique_ptr<_T1, _D1>& __x, nullptr_t) _NOEXCEPT
{
    return !__x;
}
// 复用：特殊类型 nullptr_t
template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator==(nullptr_t, const unique_ptr<_T1, _D1>& __x) _NOEXCEPT
{
    return !__x;
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(const unique_ptr<_T1, _D1>& __x, nullptr_t) _NOEXCEPT
{
    return static_cast<bool>(__x);
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(nullptr_t, const unique_ptr<_T1, _D1>& __x) _NOEXCEPT
{
    return static_cast<bool>(__x);
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<(const unique_ptr<_T1, _D1>& __x, nullptr_t)
{
    typedef typename unique_ptr<_T1, _D1>::pointer _P1;
    return less<_P1>()(__x.get(), nullptr);
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<(nullptr_t, const unique_ptr<_T1, _D1>& __x)
{
    typedef typename unique_ptr<_T1, _D1>::pointer _P1;
    return less<_P1>()(nullptr, __x.get());
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>(const unique_ptr<_T1, _D1>& __x, nullptr_t)
{
    return nullptr < __x;
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>(nullptr_t, const unique_ptr<_T1, _D1>& __x)
{
    return __x < nullptr;
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<=(const unique_ptr<_T1, _D1>& __x, nullptr_t)
{
    return !(nullptr < __x);
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<=(nullptr_t, const unique_ptr<_T1, _D1>& __x)
{
    return !(__x < nullptr);
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>=(const unique_ptr<_T1, _D1>& __x, nullptr_t)
{
    return !(__x < nullptr);
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>=(nullptr_t, const unique_ptr<_T1, _D1>& __x)
{
    return !(nullptr < __x);
}
```

以上。