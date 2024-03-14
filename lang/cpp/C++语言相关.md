[toc]

## 关于不同类型的继承

- public继承一般为了用于is-a，或者说Derived是一种Base，Derived可以改写Base的方法。主要用于多态。
- 组合是类X中有一个成员属于另一个类B的意思。
- private继承，或者protected继承，是为了用于is-implement-as，这种方式最好是用于继承接口基类，这种继承又叫做实现继承。
- virtual继承用于消除重复基类。

## C++11提供了new(std::nothrow)，但是如何组合这个到std::make\_shared呢？

[How to combine std::make\_shared and new(std::nothrow)](https://stackoverflow.com/questions/22511226/how-to-combine-stdmake-shared-and-newstdnothrow)

## 如果强制转换一个整型到enum会怎么样？

答曰：会成功，但是不一定是一个有效的enum

参考 [Enumerations (C++)](https://docs.microsoft.com/en-us/cpp/cpp/enumerations-cpp?view=msvc-160)

## 智能指针的自定义删除器

[Custom Deleters for C++ Smart Pointers](https://www.cppstories.com/2016/04/custom-deleters-for-c-smart-pointers/)

简单说，`std::shared_ptr`，`std::unique_ptr`，都可以自定义删除代码，但是 `std::make_shared`，`std::make_unique` 不能用于自定义删除器。

```
std::shared_ptr<int> pIntPtr(new int(10), 
    [](int *pi) { delete pi; }); // deleter 
```

## C++14 才支持std::unique\_ptr捕捉

[How to capture a unique\_ptr into a lambda expression?](https://stackoverflow.com/questions/8236521/how-to-capture-a-unique-ptr-into-a-lambda-expression)

    // a unique_ptr is move-only
    auto u = make_unique<some_type>(some, parameters); 

    // move the unique_ptr into the lambda
    go.run([u = move(u)]{do_something_with(u);});

## try-catch 如何实现的

*   [Compiler Internals - How Try/Catch/Throw are Interpreted by the Microsoft Compiler](https://www.codeproject.com/articles/175482/compiler-internals-how-try-catch-throw-are-interpr)，编译器把C++的try-catch转成了crt内部的一些函数组合，以及系统层面的SEH结构\_\_try/\_\_except的形式。

## const 函数中修改变量

*   [const\_cast VS mutable ? any difference?](https://stackoverflow.com/questions/11457953/const-cast-vs-mutable-any-difference)

const成员函数中想要修改成员变量，还是使用mutable修饰符吧。

## C++ 11 中的 = delete

*   [Deleted default constructor. Objects can still be created... sometimes](https://stackoverflow.com/questions/33988297/deleted-default-constructor-objects-can-still-be-created-som)

## C++11 中的互斥量

如果一个对象要释放 `std::mutex`，那就必须保证 `std::mutex` 没有被lock，不然的话会报告断言错误。
C++11标准中说一个条件变量在wait的时候，如果对应的 `std::mutex` 没有上锁，那么行为是未定义的，所以需要使用者注意mutex的状态维护。

ref: 
- [Thread-safe stack mutex destroyed while busy](https://stackoverflow.com/questions/28529018/thread-safe-stack-mutex-destroyed-while-busy)
- [std::condition_variable::wait](https://en.cppreference.com/w/cpp/thread/condition_variable/wait)

## C++ 实现一个线程池

参考这个实现：[progschj/ThreadPool](https://github.com/progschj/ThreadPool/tree/master)

## 常见的锁

## stack unwinding 是什么意思？

C++ 中stack unwinding 一般跟异常关联。参考这个问题 [What is stack unwinding?](https://stackoverflow.com/questions/2331316/what-is-stack-unwinding/2332865#2332865) 的回答中提到的：

> **Definition**: As you create objects statically (on the stack as opposed to allocating them in the heap memory) and perform function calls, they are "stacked up".
> 
> When a scope (anything delimited by { and }) is exited (by using return XXX;, reaching the end of the scope or throwing an exception) everything within that scope is destroyed (destructors are called for everything). **This process of destroying local objects and calling destructors is called stack unwinding.**
> 

调用栈展开（stack unwinding）一般是发生在异常发生时，为了保证资源正常释放，异常所在的scope中创建的对象，以及做的函数调用涉及到的资源需要做销毁和还原的过程。

这里同时也就涉及到RAII，析构函数的异常安全问题。

同时，也可以参考微软的文档：[Exceptions and Stack Unwinding in C++](https://learn.microsoft.com/en-us/cpp/cpp/exceptions-and-stack-unwinding-in-cpp?view=msvc-170)。还有一个示例代码。

[Using noexcept](https://akrzemi1.wordpress.com/2011/06/10/using-noexcept/) 