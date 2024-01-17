*分析的源码对应的是 `Xcode 12.0.1` 中 `llvm` 关于 `libc++` 的实现，对应 `clang` 版本 `Apple clang version 12.0.0 (clang-1200.0.32.2)`。`llvm` 源码[地址](https://github.com/llvm/llvm-project)，及 `llvm` [下载地址汇总](https://releases.llvm.org/download.html)。`libc++` 的[文档](https://releases.llvm.org/16.0.0/projects/libcxx/docs/index.html)*

[toc]

*代码分析的思路是，按照 [cppreference.com](https://en.cppreference.com/) 网站上列出的智能指针接口顺序，依据 llvm 中的源代码，借助 copilot 的帮助，分类别、按序分析。*
*clang 版本信息与 `Xcode` 对应关系参考[这里](https://gist.github.com/yamaya/2924292)*
*C++的标准文档，可以在这里下载对应的草案：[C++ Standards Committee Papers](https://www.open-std.org/JTC1/SC22/WG21/docs/papers/)，搜索 `Working Draft, Standard for Programming Language C++` 找到*

# 支持的操作


成员函数：

- constructor & destructor
- operator=
- reset
- swap
- use_count
- expired
- lock
- owner_before
- owner_hash
- owner_equal

非成员函数：

- std::swap()


在 `memory` 文件中有一个简化版本的接口：

```C++
template<class T>
class weak_ptr
{
public:
    typedef T element_type;

    // constructors
    constexpr weak_ptr() noexcept;
    template<class Y> weak_ptr(shared_ptr<Y> const& r) noexcept;
    weak_ptr(weak_ptr const& r) noexcept;
    template<class Y> weak_ptr(weak_ptr<Y> const& r) noexcept;
    weak_ptr(weak_ptr&& r) noexcept;                      // C++14
    template<class Y> weak_ptr(weak_ptr<Y>&& r) noexcept; // C++14

    // destructor
    ~weak_ptr();

    // assignment
    weak_ptr& operator=(weak_ptr const& r) noexcept;
    template<class Y> weak_ptr& operator=(weak_ptr<Y> const& r) noexcept;
    template<class Y> weak_ptr& operator=(shared_ptr<Y> const& r) noexcept;
    weak_ptr& operator=(weak_ptr&& r) noexcept;                      // C++14
    template<class Y> weak_ptr& operator=(weak_ptr<Y>&& r) noexcept; // C++14

    // modifiers
    void swap(weak_ptr& r) noexcept;
    void reset() noexcept;

    // observers
    long use_count() const noexcept;
    bool expired() const noexcept;
    shared_ptr<T> lock() const noexcept;
    template<class U> bool owner_before(shared_ptr<U> const& b) const noexcept;
    template<class U> bool owner_before(weak_ptr<U> const& b) const noexcept;
};

// weak_ptr specialized algorithms:
template<class T> void swap(weak_ptr<T>& a, weak_ptr<T>& b) noexcept;
```

# 具体分析

## 成员函数

### constructor & destructor

```C++
template<class _Tp>
inline
_LIBCPP_CONSTEXPR
weak_ptr<_Tp>::weak_ptr() _NOEXCEPT
    : __ptr_(0),
      __cntrl_(0)
{
}

template<class _Tp>
inline
weak_ptr<_Tp>::weak_ptr(weak_ptr const& __r) _NOEXCEPT
    : __ptr_(__r.__ptr_),
      __cntrl_(__r.__cntrl_)
{
    if (__cntrl_)
        __cntrl_->__add_weak();
}

template<class _Tp>
template<class _Yp>
inline
weak_ptr<_Tp>::weak_ptr(shared_ptr<_Yp> const& __r,
                        typename enable_if<is_convertible<_Yp*, _Tp*>::value, __nat*>::type)
                         _NOEXCEPT
    : __ptr_(__r.__ptr_),
      __cntrl_(__r.__cntrl_)
{
    if (__cntrl_)
        __cntrl_->__add_weak();
}

template<class _Tp>
template<class _Yp>
inline
weak_ptr<_Tp>::weak_ptr(weak_ptr<_Yp> const& __r,
                        typename enable_if<is_convertible<_Yp*, _Tp*>::value, __nat*>::type)
         _NOEXCEPT
    : __ptr_(__r.__ptr_),
      __cntrl_(__r.__cntrl_)
{
    if (__cntrl_)
        __cntrl_->__add_weak();
}

#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES

template<class _Tp>
inline
weak_ptr<_Tp>::weak_ptr(weak_ptr&& __r) _NOEXCEPT
    : __ptr_(__r.__ptr_),
      __cntrl_(__r.__cntrl_)
{
    __r.__ptr_ = 0;
    __r.__cntrl_ = 0;
}

template<class _Tp>
template<class _Yp>
inline
weak_ptr<_Tp>::weak_ptr(weak_ptr<_Yp>&& __r,
                        typename enable_if<is_convertible<_Yp*, _Tp*>::value, __nat*>::type)
         _NOEXCEPT
    : __ptr_(__r.__ptr_),
      __cntrl_(__r.__cntrl_)
{
    __r.__ptr_ = 0;
    __r.__cntrl_ = 0;
}

#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES

template<class _Tp>
weak_ptr<_Tp>::~weak_ptr()
{
    if (__cntrl_)
        __cntrl_->__release_weak();
}
```

- 构造函数有：

  - 默认构造函数：`weak_ptr()`
  - 拷贝构造函数：`weak_ptr(weak_ptr const& __r)`
  - 基于 `shared_ptr` 派生类对象的构造函数：`weak_ptr(shared_ptr<_Yp> const& __r, typename enable_if<is_convertible<_Yp*, _Tp*>::value, __nat*>::type)`
  - 基于 `weak_ptr` 派生类对象的构造函数：`weak_ptr(weak_ptr<_Yp> const& __r, typename enable_if<is_convertible<_Yp*, _Tp*>::value, __nat*>::type)`
  - 移动构造函数：`weak_ptr(weak_ptr&& __r)`
  - 基于 `weak_ptr` 派生类对象的构造函数：`weak_ptr(weak_ptr<_Yp>&& __r, typename enable_if<is_convertible<_Yp*, _Tp*>::value, __nat*>::type)`

从上面的总结可以看出，一个 `weak_ptr` 要么是空对象，要么维护的资源是来自于 `shared_ptr`。

- 析构函数：
  - `~weak_ptr()`

#### 其他成员函数

- 赋值操作符：
  - 同类型赋值操作符：`operator=(weak_ptr const& __r)`
  - 支持派生类对象的赋值操作符：`operator=(weak_ptr<_Yp> const& __r)`
  - 移动赋值操作符：`operator=(weak_ptr&& __r)`
  - 支持派生类对象的移动赋值操作符：`operator=(weak_ptr<_Yp>&& __r)`
  - 基于 `shared_ptr` 派生类对象的赋值操作符：`operator=(shared_ptr<_Yp> const& __r)`

```C++
template<class _Tp>
inline
weak_ptr<_Tp>&
weak_ptr<_Tp>::operator=(weak_ptr const& __r) _NOEXCEPT
{
    weak_ptr(__r).swap(*this);
    return *this;
}

template<class _Tp>
template<class _Yp>
inline
typename enable_if
<
    is_convertible<_Yp*, _Tp*>::value,
    weak_ptr<_Tp>&
>::type
weak_ptr<_Tp>::operator=(weak_ptr<_Yp> const& __r) _NOEXCEPT
{
    weak_ptr(__r).swap(*this);
    return *this;
}

#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES

template<class _Tp>
inline
weak_ptr<_Tp>&
weak_ptr<_Tp>::operator=(weak_ptr&& __r) _NOEXCEPT
{
    weak_ptr(_VSTD::move(__r)).swap(*this);
    return *this;
}

template<class _Tp>
template<class _Yp>
inline
typename enable_if
<
    is_convertible<_Yp*, _Tp*>::value,
    weak_ptr<_Tp>&
>::type
weak_ptr<_Tp>::operator=(weak_ptr<_Yp>&& __r) _NOEXCEPT
{
    weak_ptr(_VSTD::move(__r)).swap(*this);
    return *this;
}

#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES

template<class _Tp>
template<class _Yp>
inline
typename enable_if
<
    is_convertible<_Yp*, _Tp*>::value,
    weak_ptr<_Tp>&
>::type
weak_ptr<_Tp>::operator=(shared_ptr<_Yp> const& __r) _NOEXCEPT
{
    weak_ptr(__r).swap(*this);
    return *this;
}
```

- swap

上面赋值操作符中使用最多的函数 `swap`，具体的实现跟 `unique_ptr`, `shared_ptr` 一样，只是交换两个成员变量：

```C++
template<class _Tp>
inline
void
weak_ptr<_Tp>::swap(weak_ptr& __r) _NOEXCEPT
{
    _VSTD::swap(__ptr_, __r.__ptr_);
    _VSTD::swap(__cntrl_, __r.__cntrl_);
}
```

- reset

跟 `unique_ptr`, `shared_ptr` 一样，转换为与一个空对象的 `swap` 操作：

```C++
template<class _Tp>
inline
void
weak_ptr<_Tp>::reset() _NOEXCEPT
{
    weak_ptr().swap(*this);
}
```

- lock

实现的细节最终转换为了 `__shared_weak_count::lock`：

```C++
template<class _Tp>
shared_ptr<_Tp>
weak_ptr<_Tp>::lock() const _NOEXCEPT
{
    shared_ptr<_Tp> __r;
    __r.__cntrl_ = __cntrl_ ? __cntrl_->lock() : __cntrl_;
    if (__r.__cntrl_)
        __r.__ptr_ = __ptr_;
    return __r;
}
```

这里的 `__cntrl` 是 `__shared_weak_count` 类型，其 `lock` 实现：

```C++
template<class _Tp>
shared_ptr<_Tp>
weak_ptr<_Tp>::lock() const _NOEXCEPT
{
    shared_ptr<_Tp> __r;
    __r.__cntrl_ = __cntrl_ ? __cntrl_->lock() : __cntrl_;
    if (__r.__cntrl_)
        __r.__ptr_ = __ptr_;
    return __r;
}
```



- use_count

```C++
_LIBCPP_INLINE_VISIBILITY
long use_count() const _NOEXCEPT
    {return __cntrl_ ? __cntrl_->use_count() : 0;}
```

- expired

```C++
_LIBCPP_INLINE_VISIBILITY
bool expired() const _NOEXCEPT
    {return __cntrl_ == 0 || __cntrl_->use_count() == 0;}
```

- owner_before

```C++
template<class _Up>
    _LIBCPP_INLINE_VISIBILITY
    bool owner_before(const weak_ptr<_Up>& __r) const _NOEXCEPT
    {return __cntrl_ < __r.__cntrl_;}
```

`owner_before` 是用来在标准库的算法(比如 `std::owner_less`)中排序用的，一旦有了排序的算法，`shared_ptr`, `weak_ptr` 就可以作为 `key` 保存在排序的容器中了。

## 非成员函数

- swap

全局范围的 `swap` 跟 `unique_ptr`, `shared_ptr` 一样，只是转换为调用成员函数：

```C++
template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
void
swap(weak_ptr<_Tp>& __x, weak_ptr<_Tp>& __y) _NOEXCEPT
{
    __x.swap(__y);
}
```

# 需要注意的点

- 构造函数分2种：
  - 构造空对象
  - 构造一个资源来自 `shared_ptr` 或者 `weak_ptr` 的对象。
- `weak_ptr` 
  - 没有 `get`, `release` 成员函数
  - 不支持 `operator bool()`
  - 不支持 `operator <<`