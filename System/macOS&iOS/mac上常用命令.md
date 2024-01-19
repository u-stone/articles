plutil .  操作plist文件
asctl .  Apple沙盒控制工具
codesign .  用于开发者给自己的程序签名，验证签名：codesign --display --verbose=99 --entitlements=:-   \/Applications/CCtalk.app
                    查看一个已经签名的app内的entitlements文件内容：codesign -d --entitlements  -   /Applications/CCtalk.app
csreq .  管理代码签名请求数据
dtrace 调试工具. 内部使用D语言。
dtruss.是一个底层使用dtrace的工具，可以分析性能，系统调用……
otool 反汇编工具，查看依赖库
nm 反汇编工具
objdump 反汇编工具，
spindump 程序hang的时候可以得到当前的callback stack.
lsof 列举当前被打开的所有文件
fuser 查看文件被哪个进程使用
tccutil 控制app对设备的访问权限
install_name_tool  修改动态库的路径/名字
dwarfdump 输出dwarf文件的调试信息
spindump
tailspin
c++filt  反解C++符号demanagle： 

xcode-select 为Xcode和BSD工具选择开发目录。默认安装Xcode的话是/Applications/Xcode.app/Contents/Developer，安装了Command Line Tools的话，需要切换到/Library/Developer/CommandLineTools。
security keychain和安全框架的命令行接口，比如 security find-identity -v -p codesigning 可以列举出所有可用的证书名

mac上命令行：行首Ctrl+A .   行尾Ctrl+E

csrutil 设置系统安全策略




定位符号的工具：[inket/MacSymbolicator](https://github.com/inket/MacSymbolicator)
开源的崩溃用例：[bitstadium/CrashProbe](https://github.com/bitstadium/CrashProbe)


[Loading Dynamic Libraries on Mac](http://clarkkromenaker.com/post/library-dynamic-loading-mac/)

ips文件转crash文件：
- [Script for converting .ips files from new Apple JSON Crash format (used on iOS15 devices) to old style type](https://iosexample.com/script-for-converting-ips-files-from-new-apple-json-crash-format-used-on-ios15-devices-to-old-style-type/)
- [packagesdev/ips2crash](https://github.com/packagesdev/ips2crash)
- [Understanding and Deciphering .ips Crash Files on macOS 12 (Monterey)](https://copyprogramming.com/howto/how-to-read-translate-macos-12-monterey-ips-crash-files)

