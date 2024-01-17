[toc]

# macOS上进程崩溃时生成dump文件

macOS系统上，程序发生崩溃的时候，默认会生成一个.crash文件。如果想要更多的进程active信息，我们需要core dump。

默认macOS系统有一个目录 `/cores`，但是需要app开启dump功能，才会在这个目录下生成对应的core dump文件。

方法如下：

## 首先确保 `/cores` 目录存在，并且有相应权限

ref：[Core Dumps](https://developer.apple.com/library/archive/technotes/tn2124/_index.html#//apple_ref/doc/uid/DTS10003391-CH1-SECCOREDUMPS)，有些信息过期了，比如现在10.15.6上面没有 `/etc/launchd.conf` 文件。

```
保证系统中存在 /cores 目录
$ sudo mkdir /cores 

/cores 设置必要的属性
$ sudo chown root:admin /cores 
$ sudo chmod 1775 /cores
$ sudo chmod o+w /cores

/cores 默认的属性是：
$ l /cores
total 0
drwxr-xr-x   2 root  wheel    64B 12 14  2019 .
drwxr-xr-x  20 root  wheel   640B  1  1  2020 ..

/cores 经过上面的设置之后是：
$ l /cores
total 0
drwxrwxrwt   4 root  admin   128B Jul 11 18:06 .
drwxr-xr-x  23 root  wheel   736B Aug 22  2020 ..
```

到底上面这个wheel是什么，参考 [What is the "wheel" user in OS X?](https://newbedev.com/what-is-the-wheel-user-in-os-x)

还需要确认下：

```
同时，还需要查看系统是否开启core dump功能。
$ sudo sysctl kern.coredump=1
也可以先通过命令查看：
$ sysctl kern.coredump
输出为1表示开启，输出为0表示未开启。
不过我发现目前这个默认是开启的。
```

## 进程开启写dump

```
通过命令行设置系统层面的core dump文件大小，先查看目前的设置：
$ ulimit -c
如果输出是0，那么表示没有开启core dump。设置不限大小的方法是：
$ ulimit -c unlimited
再次使用 ulimit -c 查看将会输出unlimited。

但是目前这个设置，在我的系统上macOS 10.15.6上好像不起作用了。需要像下面这样在需要开启的进程中通过调用系统接口设置开启/关闭。
```

ref: [[Resolved] App crashes when launched from dock, doesn't crash when run from terminal](https://developer.apple.com/forums/thread/127503?answerId=401103022#401103022)

实验有效的方法是：

```c
struct rlimit limit;
BOOL success = getrlimit(RLIMIT_CORE, &limit) >= 0;
assert(success);
limit.rlim_cur = limit.rlim_max;
success = setrlimit(RLIMIT_CORE, &limit) >= 0;
assert(success);
```

不过 [Mac OS X Debugging Magic](https://developer.apple.com/library/archive/technotes/tn2124/_index.html#//apple_ref/doc/uid/DTS10003391-CH1-SECCOREDUMPS) 中给的写法是：

```
#include <sys/resource.h>

static bool EnableCoreDumps(void)
{
    struct rlimit   limit;

    limit.rlim_cur = RLIM_INFINITY;
    limit.rlim_max = RLIM_INFINITY;
    return setrlimit(RLIMIT_CORE, &limit) == 0;
}
```

通过上面的设置之后，如果macOS系统中的进程发生崩溃，会有core dump文件在 `/cores` 目录下生成。然后就可以通过lldb，gdb等工具调试了。下面是出现dump时，/cores目录下文件结构。

```
➜  ~ l /cores
total 18651416
drwxrwxrwt   4 root  admin   128B Jul 11 18:06 .
drwxr-xr-x  23 root  wheel   736B Aug 22  2020 ..
-r--------@  1 will  admin   4.3G Jul 11 18:00 core.46972
-r--------@  1 will  admin   4.6G Jul 11 18:06 core.47125
```

不过有一个问题，一旦支持生成core dump的话，在console中刷新到生成crash文件似乎要延迟较久的时间，我电脑上大约是5分钟，比如crash文件实际创建的时间是 `2021-07-11 18:06:23.162 +0800`，但是console中列出的时间是 `2021/07/11, 18:11` 。

# 内存问题

macOS上有两个现成的工具：sanitizer和malloc特性

## sanitizer

AddressSanitizer是一项编译和运行时技术，用于发现难以定位的bug。作用于编译选项，是编译器支持的。因为这也是一项运行期技术，所以对于一些第三方的二进制库，也可以定位其存在的内存问题。

Google官方文档参考：[AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer)，要认真阅读这篇文档，比如里面提到FORTIFY_SOURCE与asan/msan/tsan不能一起使用，gcc，clang编译出来的代码不要混合asan一起使用，asan默认填充申请出来的内存值为0xbe等等，还有想要一次性打印所有检测到的问题，而不是碰到一个就停下来，可以 `-fsanitize-recover=address` 配合环境变量 `ASAN_OPTIONS=halt_on_error=0`来实现。

webrtc的源码中 `src/build/config/compiler/BUILD.gn` 有一段代码:

```
if (!is_debug && !using_sanitizer &&
    (!is_linux || !is_clang || is_official_build) &&
    current_cpu != "s390x" && current_cpu != "s390" &&
    current_cpu != "ppc64" && current_cpu != "ppc64" &&
    current_cpu != "mips" && current_cpu != "mips64") {
  # _FORTIFY_SOURCE isn't really supported by Clang now, see
  # http://llvm.org/bugs/show_bug.cgi?id=16821.
  # It seems to work fine with Ubuntu 12 headers though, so use it in
  # official builds.
  #
  # Non-chromium code is not guaranteed to compile cleanly with
  # _FORTIFY_SOURCE. Also, fortified build may fail when optimizations are
  # disabled, so only do that for Release build.
  defines += [ "_FORTIFY_SOURCE=2" ]
}
```

*可以看出，sanitizer与_FORTIFY_SOURCE不同时使用*

sanitizer的详细参数控制参考：[AddressSanitizerFlags](https://github.com/google/sanitizers/wiki/AddressSanitizerFlags)。

macOS 系统上，对于没有启用沙盒的app，使用sanitizer需要安装llvm-symbolizer使得生成的报告中提供更详细的信息，否则生成的错误报告中不会报告正确的代码行数，而是一个内存地址。也就是这里描述的问题：[Symbolizing the Reports](https://clang.llvm.org/docs/AddressSanitizer.html#symbolizing-the-reports)

禁止链接的第三方二进制库文件的错误报告、禁止某些函数和源文件的错误报告、忽略某些文件的错误报告，可参考llvm的文档：[Issue Suppression](https://clang.llvm.org/docs/AddressSanitizer.html#issue-suppression)。这里提到macOS上如果开启了沙盒就无法实时(on-line)显示错误的代码行数，只能通过离线的方式（off-line）来翻译，使用的工具是dsymutil。`不过实际使用下来看，如果是使用Xcode调试的话，基本也就不需要行数的提示了，调试器会自动在出问题的地方停下来。`所以这个问题也就不需要解决了(除非是想要脚本化检测过程)。于是乎，可以直接在Xcode的scheme->Arguments->Environments Variables中添加环境变量：name填`ASAN_OPTIONS`，value填`symbolize=0`来关闭符号翻译。如果想设置其他选项，可以通过`:`连接。比如 `detect_container_overflow=0:symbolize=0:halt_on_error=0`。

结合文档 [Turning off instrumentation](https://github.com/google/sanitizers/wiki/AddressSanitizer#turning-off-instrumentation) 可以了解到设置黑名单的方法是添加clang编译选项：-fsanitize-blacklist=my_ignores.txt，my_ignores.txt的格式是：

```
# Suppress error reports for code in a file or in a function:
src:bad_file.cpp
# Ignore all functions with names containing MyFooBar:
fun:*MyFooBar*
# Disable out-of-bound checks for global:
global:bad_array
# Disable out-of-bound checks for global instances of a given class ...
type:Namespace::BadClassName
# ... or a given struct. Use wildcard to deal with anonymous namespace.
type:Namespace2::*::BadStructName
# Disable initialization-order checks for globals:
global:bad_init_global=init
type:*BadInitClassSubstring*=init
src:bad/init/files/*=init
```
> 引用自 [Suppressing Errors in Recompiled Code (Ignorelist)](https://clang.llvm.org/docs/AddressSanitizer.html#suppressing-errors-in-recompiled-code-ignorelist)，更加详细的语法规则，参考 [Sanitizer special case list](https://releases.llvm.org/11.0.0/tools/clang/docs/SanitizerSpecialCaseList.html)

webrtc中的示例代码位于: `src/tools/memory/tsan_v2/ignores.txt`

开启的方法可以分为两类：通过Xcode中的scheme，或者通过command line设置编译参数。具体参考 [Diagnosing Memory, Thread, and Crash Issues Early](https://developer.apple.com/documentation/xcode/diagnosing-memory-thread-and-crash-issues-early?preferredLanguage=occ)

webrtc工程中子工程太多，需要通过gn来设置参数。在Mac上编译webrtc并开启内存清洗器的方法是在gn gen xxx的时候，在参数中添加上一句：is_asan=true，比如：

```
gn gen out/72 --ide="xcode" --args='target_cpu="x64" is_debug=false is_component_build=false mac_deployment_target="10.11" rtc_use_h264=true rtc_enable_protobuf=true ffmpeg_branding="Chrome"  is_asan=true'
````

其余的gn的编译选项中还有很多，但是macOS基本都用不了，比如：

- 使用 is_ubsan_null，is_ubsan_vptr，is_ubsan_no_recover，use_cfi_diag的话，clang会有编译崩溃
- is_msan 只支持Linux
- is_safestack 不被clang （8.0.0）支持

sanitizer生成报告的各类错误对应的示例代码，可以参考微软的文档：[AddressSanitizer error examples](https://docs.microsoft.com/en-us/cpp/sanitizers/asan-error-examples?view=msvc-160)。Apple的文档在 [Diagnosing Memory, Thread, and Crash Issues Early](https://developer.apple.com/documentation/xcode/diagnosing-memory-thread-and-crash-issues-early) 下面的 Topic `Address Sanitizer` 小结中。推荐阅读微软的文档！！！

使用sanitizer之后，官方文档说是性能下降2倍，算是十分高效的了。

关于错误报告中的shadow bytes的概念，参考微软文档：[AddressSanitizer shadow bytes](https://docs.microsoft.com/en-us/cpp/sanitizers/asan-shadow-bytes?view=msvc-160)，核心概念是1个影子字节映射8个虚拟内存字节。计算方式：

```c
// x86
char shadow_byte_value = *((Your_Address >> 3) + 0x30000000)
// x64
char shadow_byte_value = *((Your_Address >> 3) + _asan_runtime_assigned_offset)
```

### 工程配置中会涉及到场景

- 直接对clang设置asan参数的方法：

```
// ref: https://clang.llvm.org/docs/AddressSanitizer.html#usage
clang++ -O1 -g -fsanitize=address -fno-omit-frame-pointer example_UseAfterFree.cc
```

- CMakeList.txt中控制cmake启用asan的方法：

```
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")    
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
```

- CMakeList中配置生成的Xcode工程启用asan的方法：

```
set(CMAKE_XCODE_GENERATE_SCHEME ON)
set(CMAKE_XCODE_SCHEME_ADDRESS_SANITIZER ON)
set(CMAKE_XCODE_SCHEME_ADDRESS_SANITIZER_USE_AFTER_RETURN ON)
```

- app的*.plist中配置asan的环境变量

```
<key>LSEnvironment</key>
<dict>
	<key>ASAN_OPTIONS</key>
	<string>detect_container_overflow=0</string>
</dict>
```

## malloc特性

这个是利用macOS系统上的malloc库支持的特性。参考 [Malloc Debug Environment Variables Release Notes](https://developer.apple.com/library/archive/releasenotes/DeveloperTools/RN-MallocOptions/index.html) 和 [Enabling the Malloc Debugging Features](https://developer.apple.com/library/archive/documentation/Performance/Conceptual/ManagingMemory/Articles/MallocDebug.html)

不过有一点需要说明，malloc特性开启之后，app的性能下降非常多，这一点比sanitizer差很多。

## 使用开源的 Valgrind

macOS上支持的一直不太好，目前最新版本仅仅是初步支持macOS 10.13，ref：[Valgrind Distribution Documents](https://www.valgrind.org/docs/manual/dist.news.html)。如果使用home-brew来安装Valgrind的话，会报错：

```
valgrind: Linux is required for this software.
Error: An unsatisfied requirement failed this build.
```

可以尝试自行编译，参考 [Valgrind on macOS Sierra](https://stackoverflow.com/questions/40650338/valgrind-on-macos-sierra)，成功的可能性不太大。

```
valgrind -v your-program
```

# 线程问题

[ThreadSanitizer](https://clang.llvm.org/docs/ThreadSanitizer.html)

[A little bit about Thread Sanitizer](https://medium.com/@lucianoalmeida1/i-little-bit-about-thread-sanitizer-56a887dc144)



# 参考

- [How to macOS Core Dump](https://steipete.com/posts/how-to-macos-core-dump/)，[How to generate core dump files](https://www.ibm.com/support/pages/how-generate-core-dump-files)，[Managing Core Dumps](https://krypted.com/mac-security/managing-core-dumps/)
- [Mac OS 产生 Coredump，定位 Segmentation Fault](https://blog.csdn.net/justidle/article/details/108459488)
- [10.15.5 & 10.15.4 = Wake from Sleep Kernel Panic on 16″ MBPro (2019)](https://mrmacintosh.com/10-15-4-update-wake-from-sleep-kernel-panic-in-16-mbpro-2019/)
- [Technical Note TN2118 Kernel Core Dumps](https://developer.apple.com/library/archive/technotes/tn2004/tn2118.html) 介绍了关于固件（firmware）变量 `boot-args` 的修改。
- wwdc视频：[Advanced Debugging and the Address Sanitizer](https://developer.apple.com/videos/play/wwdc2015/413/) 其中有提到malloc特性和sanitizer检测崩溃的原理
- Xcode上通过cmake来控制scheme的设置：[XCODE_GENERATE_SCHEME](https://cmake.org/cmake/help/latest/prop_tgt/XCODE_GENERATE_SCHEME.html#prop_tgt:XCODE_GENERATE_SCHEME)
- [Collecting Crash Reports, Mini-Dumps And Core Dumps](https://help.resilio.com/hc/en-us/articles/206214615-Collecting-crash-reports-mini-dumps-and-core-dumps)中的MAC OS X部分，查看其脚本会发现，也是使用了开启core dump，然后通过脚本找到对应的pid，然后一旦发现/cores目录下有core.$pid之后就压缩好等用户上传。
- [[Resolved] App crashes when launched from dock, doesn't crash when run from terminal](https://developer.apple.com/forums/thread/127503?answerId=401103022#401103022) 附上一个不错的core dump分析过程。
- [How to get a core dump for a segfault on Linux](https://jvns.ca/blog/2018/04/28/debugging-a-segfault-on-linux/) 有关于 sysctl 的更多设置。
- [定位多线程内存越界问题实践总结](https://www.cnblogs.com/djinmusic/archive/2013/02/04/2891753.html) 2013年的文章，思路可以参考，工具有些无法使用了。
- [“Electric Fence: Who Let the Heap Corruption Out?”](https://blog.securityevaluators.com/electric-fence-who-let-the-heap-corruption-out-f40454737e20)，[Electric Fence](https://elinux.org/Electric_Fence)，
- [llvm-symbolizer - convert addresses into source code locations](https://llvm.org/docs/CommandGuide/llvm-symbolizer.html) 
- [Security Technologies: FORTIFY_SOURCE](https://access.redhat.com/blogs/766093/posts/3606481)，[Toward _FORTIFY_SOURCE parity between Clang and GCC](https://developers.redhat.com/blog/2020/02/11/toward-_fortify_source-parity-between-clang-and-gcc)
- [Symbolicating iOS crash reports and logs](https://www.bugsnag.com/blog/symbolicating-ios-crashes)
- [LLVM Sanitizers](https://source.android.com/devices/tech/debug/sanitizers)