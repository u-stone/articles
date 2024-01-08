当然，事前要有一些编译原理方面基础课程的知识储备。
LLVM到底是什么：low level virtual mechine。但是后来的发展已经不是当初名字含义能覆盖的了。最当初是一个研究项目，现在是一个可用于学术研究，以及商业应用的开源框架，它有一个比较牛逼的子项目叫clang，可以用来编译C类语言，并且代码优化的还特别好，渐渐地取代了gcc/msvc的地位。
首要的还是看官方文档，这个是学习的大纲。llvm之前使用svn维护，到现在为止还没有一个正式的git仓库，不过有一个官方人员搞的非官方的git代码镜像，每5分钟更新一次。
要想编译LLVM，先下载代码，你可以使用svn下载，也可以直接下载某个版本的压缩包。
注意需要安装并配置好（系统路径）工具软件：cmake，python。直接用最新的就好。
我是在Windows上编译的，使用的编译器是vs2015对应的msvc编译器。按照[这里](http://clang.llvm.org/get_started.html)说明的方式，下载并放置好代码之后，使用cmake工具编译生成llvm.sln工程。如果是仅编译非optional的工程，需要耗费大概1个小时的样子（我的电脑是一台性能还不错的台式机，代码在ssd上，而且CPU比较强劲）；如果要编译全部工程，耗费的时间大概是2个小时。所以一旦开始编译你要做好“切换线程”的准备，比如继续看看文档。
不过文档里面的介绍的东西可能比较旧，要带着怀疑的精神来参考其内容。如果仅仅是要编译llvm及其子项目，参考[Getting Started with the LLVM System](http://llvm.org/docs/GettingStarted.html)一篇就可以了；[Getting Started: Building and Running Clang和Getting Started with the LLVM System using Microsoft Visual Studio](https://llvm.org/docs/GettingStartedVS.html)的内容不全，而且旧，不推荐参考。要是使用cmake命令行操作的话，参考Building LLVM with CMake。
Note：使用VS2015编译的话CPU会跑满，但是在遇到编译Tablegenning下的项目的时候，编译其*.inc文件会很慢。
编译好之后就可以开始开发一下实践一下llvm的能力了。看一下[LLVM Tutorial: Table of Contents](http://llvm.org/docs/tutorial/index.html)，这篇文章介绍了如何使用llvm，以及使用llvm创造一种自己的语言。听起来就很cool。
其他几篇很有用的文档：[FAQ](http://llvm.org/docs/FAQ.html)，[LLVM Command Guide](http://llvm.org/docs/CommandGuide/index.html)介绍LLVM内置的各种小工具的用法，[LLVM’s Analysis and Transform Passes](http://llvm.org/docs/Passes.html)，[LLVM Testing Infrastructure Guide](http://llvm.org/docs/TestingGuide.html)介绍了一下llvm使用的测试框架。
介绍性的文档也就上面提到的这些有料了。接下来就要开始看具体使用llvm做一点事情了。
LLVM框架介绍的文档：[Intro to LLVM](http://www.aosabook.org/en/llvm.html)，[LLVM: A Compilation Framework for Lifelong Program Analysis & Transformation](http://llvm.org/pubs/2004-01-30-CGO-LLVM.html)，[The LLVM Compiler Infrastructure](http://llvm.org/pubs/)。

编译原理一般讲到的知识点是：词法分析，语法分析，语义分析，代码生成；深层次一点的就是各类制导的翻译，以及代码优化。要在后续的学习研究中找出这些课题对应的代码模块。
具体的学习任务是
1）读完Kaleidoscope的教程
2）读完书目：《Getting started with LLVM Core Libraries》，通过使用Core Libraries熟悉LLVM的代码结构。
3）结合上面的认知，以及LLVM相关的论文，厘清编译原理与LLVM代码之间的关系；通过这一步掌握清楚LLVM的代码结构。
4）之前据听说Chris Lattner是在大学花了4年时间专研龙书《编译原理》才写的LLVM，所以还是要有一定的编译原理基础方可看懂LLVM的代码。

下面记载内容分述
1）Kaleidoscope教程中提到的Wiki文章
[Writing an LLVM Pass](http://llvm.org/docs/WritingAnLLVMPass.html)   这里的Pass我查了一下词典，感觉可以翻译为“阶段、步骤”；LLVM中有Analysis Pass（代码分析阶段），Transform Pass（代码变形阶段）以及Utility Pass（杂项）。参考[LLVM’s Analysis and Transform Passes](http://llvm.org/docs/Passes.html)。
Kaleidoscope的教程看起来云里雾里的，只是告诉你如何做就可以得到什么结果，缺乏解释性的东西；决定先看书，转入第二步。

2）LLVM的重要概念：
frontend：这是一个编译步骤，将C/C++，java这类计算机语言翻译为LLVM的IR，包含了一个词法分析器，语法分析器，语义分析器，和一个LLVM IR代码生成工具。
IR：有可读的和二进制加密的两种格式，工具和库中包含了IR的构造、汇编反汇编的接口，LLVM的优化器也会使用IR。关于LLVM的IR有一个专门的文档：[LLVM Language Reference Manual](http://llvm.org/docs/LangRef.html)。
backend：这一步骤中，IR将转为目标代码，比如汇编代码、或者其他对象代码。
pass：是一个transformation分析或者优化。
LLVM中的数据交互，模块内部一般是in-memory，独立的、较小的模块间、或者用户输入的文件一般是through-files

关于LLVM的pass参考油管上的这个视频的[教程](https://www.youtube.com/channel/UCO4qrmuvmG-fwjsQCYlOyKQ/videos)（部分视频）。







资料：
LLVM的[周报](http://llvmweekly.org/)，有人汇总的。
知乎上蓝色的专栏：[蓝色的味道](https://zhuanlan.zhihu.com/frozengene)
台湾国立交通大学的一个[Wiki](https://people.cs.nctu.edu.tw/~chenwj/dokuwiki/doku.php?id=llvm)，汇总了不少资料。
Eli Bendersky 博客中关于[LLVM&Clang](https://eli.thegreenplace.net/tag/llvm-clang)的部分



官方资料：
[LLVM开发手册](http://www.llvm.org/docs/ProgrammersManual.html) （[LLVM Programmer’s Manual](http://llvm.org/docs/ProgrammersManual.html)），不过只是介绍了常用的类和接口，并不是一个入门的手册，反而是假定你对LLVM有了一定的了解之后，想要进一步掌握代码，或者是拿LLVM做一些事情。
LLVM的术语：[The LLVM Lexicon](http://www.llvm.org/docs/Lexicon.html)
