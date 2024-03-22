# auto

# 右值引用

# lambda 表达式

lambda 表达式的参数捕获中要避免出现“引用悬挂”问题。这种一般出现在lambda表达式应用了一个局部变量，但是lambda表达式被拷贝到另外一个作用域。

一般来说，最好是显式地列出依赖的局部变量或者参数。

捕获只能应用于lambda被创建时所在作用域里的non-static局部变量（包括形参）。[条款三十一：避免使用默认捕获模式](https://cntransgroup.github.io/EffectiveModernCppChinese/6.LambdaExpressions/item31.html)

lambda 表达式的实现是匿名类。

```C++
int test(int a = 0, int b = 0) {
auto lambda = [a, b](){
    return a + b;
};
    return lambda();
}

int main() {
    int a = 1, b = 2;
    test(a, b);
}
```

对应的汇编代码：

```

        ; ================ B E G I N N I N G   O F   P R O C E D U R E ================

        ; Variables:
        ;    var_8: int64_t, -8


                     __ZZ4testiiENK3$_0clEv:        // test(int, int)::$_0::operator()() const
0000000100003f60         push       rbp                                         ; CODE XREF=__Z4testii+30
0000000100003f61         mov        rbp, rsp
0000000100003f64         mov        qword [rbp+var_8], rdi
0000000100003f68         mov        rax, qword [rbp+var_8]
0000000100003f6c         mov        ecx, dword [rax]
0000000100003f6e         add        ecx, dword [rax+4]
0000000100003f71         mov        eax, ecx
0000000100003f73         pop        rbp
0000000100003f74         ret
                        ; endp
0000000100003f75         align      128
```

注意观察lambda表达式的签名：`test(int, int)::$_0::operator()() const`，按照重载调用符的格式：`test(int, int)` 是namespace，`$_0` 是类名称，`operator()() const` 是重载符号的声明。

如果把这个lambda表达式放在main函数中，这个签名就会变成 `main()::$_0::operator()() const`。

# 智能指针

std::auto_ptr, unique_ptr, shared_ptr, weak_ptr 都要看一下。尤其是 shared_ptr 和 weak_ptr 的交互操作。比如 weak_ptr 的lock如何实现？

std库中的智能指针一般都带有一个删除器，不过一般的C++面试中不会要求你写一个删除器版本的智能指针。网上搜索到的很多代码也都是直接做 new/delete 的。

可以参考几篇文章：

- [【c++面试之智能指针】【万字长文】【超级详细】最详细解析四种智能指针，面试题汇总及解答，妈妈再也不担心面试官考我智能指针了。](https://zhuanlan.zhihu.com/p/609599904)
- [C++面试题（四）——智能指针的原理和实现](https://blog.csdn.net/worldwindjp/article/details/18843087) 
- [C++面试八股文：什么是智能指针？](https://juejin.cn/post/7246328828838936637)

一般来说，std::weak_ptr 可以打破 std::shared_ptr 的循环引用，不过还有一个用处，比如说 std::shared_ptr 维护的一个资源，只是为了方便持有和释放，但是有另外一个地方想要在任何时候知道该资源是否有效，如果有效，再通过 std::weak_ptr::lock 方法构造一个 std::shared_ptr 出来使用，那么这时候就比较适合用 std::weak_ptr 了。 这个例子在 `Effective Modern C++` 中的 [条款二十：当std::shared_ptr可能悬空时使用std::weak_ptr](https://cntransgroup.github.io/EffectiveModernCppChinese/4.SmartPointers/item20.html) 有提到。

