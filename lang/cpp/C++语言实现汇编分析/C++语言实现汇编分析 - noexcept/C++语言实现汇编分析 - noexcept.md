
TODO: 

## 背景资料

写这类文章之前，还是需要了解一些 C++ 设计方面的背景知识。 

- [F.6: If your function must not throw, declare it noexcept](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f6-if-your-function-must-not-throw-declare-it-noexcept)
- [Does noexcept improve performance?](https://stackoverflow.com/questions/16104057/does-noexcept-improve-performance)
- [Using noexcept](https://akrzemi1.wordpress.com/2011/06/10/using-noexcept/) 
- [noexcept — what for?](https://akrzemi1.wordpress.com/2014/04/24/noexcept-what-for/)

## 开始分析

msvc 中写出如下代码：

```c++
int test_exception(int a, int b) {
  return a * b;
}

int test_exception_noexcept(int a, int b) noexcept {
  return a * b;
}

int main() {
  test_exception(1, 2);
  test_exception_noexcept(3, 4);
  return 0;
}
```

编译的时候设置不多的优化级别，生成的汇编不一样，下面是设置了 `/O1` 的。

使用 IDA 打开之后，稍微修改一下routine的命名，可以看到 `test_exception` 的实现：


下面是 test_exception_noexcept 的汇编：
