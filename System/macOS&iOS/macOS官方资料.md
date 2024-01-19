[TOC]

# WWDC

[YouTube上 Apple WWDC Videos](https://www.youtube.com/channel/UCOYNV2EifmBprMdSESVEu9g/playlists)，这里有最早1997年的视频。

# 工具

*   [Xcode Overview](https://developer.apple.com/library/archive/documentation/ToolsLanguages/Conceptual/Xcode_Overview/index.html)
*   [Xcode Help](https://help.apple.com/xcode/mac/current/) 信息比较详细
*   [Xcode](https://developer.apple.com/documentation/xcode?language=objc) 全面介绍：调试，性能优化，测试，发布……
    *   [Adding Identifiable Symbol Names to a Crash Report](https://developer.apple.com/documentation/xcode/diagnosing_issues_using_crash_reports_and_device_logs/adding_identifiable_symbol_names_to_a_crash_report?language=objc)  为崩溃文件添加符号
*   [Information Property List](https://developer.apple.com/documentation/bundleresources/information_property_list?language=objc) plist可以配置的项目。

# 调试

*   [Technical Note TN2124 Mac OS X Debugging Magic](https://developer.apple.com/library/archive/technotes/tn2124/_index.html)
*   [Instruments Help](https://help.apple.com/instruments/mac/current/)
*   [Debugging with Xcode](https://developer.apple.com/library/archive/documentation/DeveloperTools/Conceptual/debugging_with_xcode/chapters/about_debugging_w_xcode.html)
*   [Diagnosing Issues Using Crash Reports and Device Logs](https://developer.apple.com/documentation/xcode/diagnosing_issues_using_crash_reports_and_device_logs?language=objc)
    *   [Adding Identifiable Symbol Names to a Crash Report](https://developer.apple.com/documentation/xcode/diagnosing_issues_using_crash_reports_and_device_logs/adding_identifiable_symbol_names_to_a_crash_report?language=objc)
    *   [Identifying the Cause of Common Crashes](https://developer.apple.com/documentation/xcode/diagnosing_issues_using_crash_reports_and_device_logs/identifying_the_cause_of_common_crashes?language=objc)
*   [Diagnosing Memory, Thread, and Crash Issues Early](https://developer.apple.com/documentation/xcode/diagnosing_memory_thread_and_crash_issues_early?language=objc) 帮助理解Xcode中Scheme > Diagnostics的一些设置的含义。
    *   Address Sanitizer
    *   Thread Sanitizer
    *   Undefined Behavior Sanitizer
    *   Main Thread Checker
*   [LLDB Quick Start Guide](https://developer.apple.com/library/archive/documentation/IDEs/Conceptual/gdb_to_lldb_transition_guide/document/Introduction.html)
*   [LLDB Debugging Guide](https://developer.apple.com/library/archive/documentation/General/Conceptual/lldb-guide/chapters/Introduction.html)
*   [Enabling the Malloc Debugging Features](https://developer.apple.com/library/archive/documentation/Performance/Conceptual/ManagingMemory/Articles/MallocDebug.html)，[Malloc Debug Environment Variables Release Notes](https://developer.apple.com/library/archive/releasenotes/DeveloperTools/RN-MallocOptions/index.html) 这篇文档里面有malloc特性的详细解释。
*   [Memory Usage Performance Guidelines](https://developer.apple.com/library/archive/documentation/Performance/Conceptual/ManagingMemory/Articles/MallocDebug.html)
*   [Advanced Debugging and the Address Sanitizer](https://developer.apple.com/videos/play/wwdc2015/413/)
*   [Practical Approaches to Great App Performance](https://developer.apple.com/videos/play/wwdc2018/407/) 中12：00左右开始介绍timer profile工具，2016年的wwdc本来也有一个叫做System trace in depth的视频介绍timer profiler的，详细介绍了工作原理（采样call stack，最终做一个call tree的统计计数），但是这个视频被删除了。

# 内核

*   [Kernel Programming Guide](https://developer.apple.com/library/archive/documentation/Darwin/Conceptual/KernelProgramming/About/About.html) 比较旧的文档，内有 BSD与OS X的异同，以及如何调试内核。OS X内核是基于free BSD 4.4派生出来的一个版本。
*   [Kernel](https://developer.apple.com/documentation/kernel) 比较新的文档，但是这个文档是那种API罗列的文档，十分“苹果”
*   [Understanding and Debugging Kernel Panics](https://developer.apple.com/library/archive/technotes/tn2063/_index.html)
*   [IOKit Fundamentals](https://developer.apple.com/library/archive/documentation/DeviceDrivers/Conceptual/IOKitFundamentals/Introduction/Introduction.html)
*   [Accessing Hardware From Applications](https://developer.apple.com/library/archive/documentation/DeviceDrivers/Conceptual/AccessingHardware/AH_Intro/AH_Intro.html)
*   [Kernel Extension Programming Topics](https://developer.apple.com/library/archive/documentation/Darwin/Conceptual/KEXTConcept/KEXTConceptIntro/introduction.html)

macOS的kernel就是[xnu](https://opensource.apple.com/source/xnu/)，而这个是开源的。可以在[这里](https://opensource.apple.com/source/)搜索任何感兴趣的内容，比如webkit，dtrace，gperg，lldb，top，vim……。

## 外部资料

*   [FreeBSD Developers' Handbook](https://docs.freebsd.org/en/books/developers-handbook/index.html)

# 权限管理

*   [Authorization Services Programming Guide](https://developer.apple.com/library/archive/documentation/Security/Conceptual/authorization_concepts/02authconcepts/authconcepts.html)

# 异常

*   [Error Handling Programming Guide](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/ErrorHandlingCocoa/ErrorHandling/ErrorHandling.html)
*   [Exception Programming Topics](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/Exceptions/Exceptions.html)
*   [Analyzing a Crash Report](https://developer.apple.com/documentation/xcode/analyzing-a-crash-report) 分析崩溃日志的汇总性文档
*   [Understanding Crashes and Crash Logs](https://developer.apple.com/videos/play/wwdc2018/414/) WWDC2018一篇不错的介绍crash文件分析方法的视频，其中使用lldb加载crash文件，定位偏移量的解决方案很酷。
*   [Technical Q\&A QA1184 Signals and Threads](https://developer.apple.com/library/archive/qa/qa2001/qa1184.html#//apple_ref/doc/uid/DTS10001713) Mac OS X 10.2中信号与线程的关系。
*   [Memory Usage Performance Guidelines](https://developer.apple.com/library/archive/documentation/Performance/Conceptual/ManagingMemory/ManagingMemory.html)

# 卡死
*   [Track down hangs with Xcode and on-device detection](https://developer.apple.com/videos/play/wwdc2022/10082/)
*   [Understand and eliminate hangs from your app](https://developer.apple.com/videos/play/wwdc2021/10258/)
*   [Diagnose Power and Performance regressions in your app](https://developer.apple.com/videos/play/wwdc2021/10087)
*   [System Trace in Depth](https://developer.apple.com/videos/play/wwdc2016/411)

# 框架 & 语言

*   [Programming with Objective-C](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/ProgrammingWithObjectiveC/Introduction/Introduction.html)
*   [Core Graphics](https://developer.apple.com/documentation/coregraphics?language=objc)
*   [Cocoa Event Handling Guide](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/EventOverview/Introduction/Introduction.html)
*   [String Programming Guide](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/Strings/introStrings.html)
*   [OS X Assembler Reference](https://developer.apple.com/library/archive/documentation/DeveloperTools/Reference/Assembler/000-Introduction/introduction.html)

## Metal

*   [Metal Programming Guide](https://developer.apple.com/library/archive/documentation/Miscellaneous/Conceptual/MetalProgrammingGuide/Introduction/Introduction.html)
*   [Metal Best Practices Guide](https://developer.apple.com/library/archive/documentation/3DDrawing/Conceptual/MTLBestPracticesGuide/index.html)

## MetricKit

聚合和分析每个设备关于异常和崩溃诊断、电源和性能指标的报告。

*   [MetricKit](https://developer.apple.com/documentation/metrickit?language=objc)
*   [Improving Your App's Performance](https://developer.apple.com/documentation/xcode/improving_your_app_s_performance?language=objc)
*   [Gathering Information About Memory Use](https://developer.apple.com/documentation/xcode/improving_your_app_s_performance/reducing_your_app_s_memory_use/gathering_information_about_memory_use?language=objc)

## Quartz

*   Display Mode是什么：[Quartz Display Services Programming Topics](https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/QuartzDisplayServicesConceptual/Articles/DisplayModes.html#//apple_ref/doc/uid/TP40004234)，[Display Mode Standard Properties](https://developer.apple.com/documentation/coregraphics/quartz_display_services/display_mode_standard_properties?language=objc)
*   [Quartz 2D Programming Guide](https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/Introduction/Introduction.html)，中文版的一个翻译： [Quartz 2D编程指南](http://southpeak.github.io/categories/translate/)

## XCTest

## Audio & Video

*   [Getting Started with Audio & Video](https://developer.apple.com/library/archive/referencelibrary/GettingStarted/GS_MusicAudio/_index.html)
*   [Audio & Video Starting Point](https://developer.apple.com/library/archive/referencelibrary/GettingStarted/AudioVideoStartingPoint_iOS/index.html#//apple_ref/doc/uid/TP40007298)
*   [Multimedia Programming Guide](https://developer.apple.com/library/archive/documentation/AudioVideo/Conceptual/MultimediaPG/Introduction/Introduction.html)
*   [Media Playback Programming Guide](https://developer.apple.com/library/archive/documentation/AudioVideo/Conceptual/MediaPlaybackGuide/Contents/Resources/en.lproj/Introduction/Introduction.html)
*   [AVFoundation Programming Guide](https://developer.apple.com/library/archive/documentation/AudioVideo/Conceptual/AVFoundationPG/Articles/00_Introduction.html)
*   [Media Playback Programming Guide](https://developer.apple.com/library/archive/documentation/AudioVideo/Conceptual/MediaPlaybackGuide/Contents/Resources/en.lproj/Introduction/Introduction.html)
*   [Core Video Programming Guide](https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/CoreVideo/CVProg_Tasks/CVProg_Tasks.html)
*   [Core Image Programming Guide](https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/CoreImaging/ci_intro/ci_intro.html)
*   [Core Animation Programming Guide](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/CoreAnimation_guide/Introduction/Introduction.html)

音频的资料，在[Documentation Archive](https://developer.apple.com/library/archive/navigation/)上搜audio，并且以guides过滤，可以找到下面的大部分：

*   **[Working with Audio](https://developer.apple.com/audio/)** 新版
*   **[Core Audio Overview](https://developer.apple.com/library/archive/documentation/MusicAudio/Conceptual/CoreAudioOverview/Introduction/Introduction.html)**
*   **[Audio Session Programming Guide](https://developer.apple.com/library/archive/documentation/Audio/Conceptual/AudioSessionProgrammingGuide/Introduction/Introduction.html)**
*   **[Apple Media Service Reference](https://developer.apple.com/library/archive/documentation/CoreBluetooth/Reference/AppleMediaService_Reference/Introduction/Introduction.html)**
*   **[Audio Unit](https://developer.apple.com/documentation/audiounit?language=objc) V3**，这里推荐先阅读：[App Extension Programming Guide](https://developer.apple.com/library/archive/documentation/General/Conceptual/ExtensibilityPG/index.html) 和 [Audio Unit Extensions](https://developer.apple.com/videos/play/wwdc2015/508/)
*   [Audio Unit Programming Guide](https://developer.apple.com/library/archive/documentation/MusicAudio/Conceptual/AudioUnitProgrammingGuide/Introduction/Introduction.html) V2 (老了一个版本)
*   [App Extension Programming Guide](https://developer.apple.com/library/archive/documentation/General/Conceptual/ExtensibilityPG/index.html)
*   [Audio Queue Services Programming Guide](https://developer.apple.com/library/archive/documentation/MusicAudio/Conceptual/AudioQueueProgrammingGuide/Introduction/Introduction.html) 已停止更新
*   [Core Audio Glossary](https://developer.apple.com/library/archive/documentation/MusicAudio/Reference/CoreAudioGlossary/Introduction/Introduction.html) 已停止更新
*   [~~Audio & Video Starting Point~~](https://developer.apple.com/library/archive/referencelibrary/GettingStarted/AudioVideoStartingPoint_iOS/index.html)~~ 过时的文档~~
*   [Getting Started with Audio & Video](https://developer.apple.com/library/archive/referencelibrary/GettingStarted/GS_MusicAudio/_index.html) 已停止更新
*   [Core Audio Data Types Reference](https://developer.apple.com/documentation/coreaudio/core_audio_data_types)
*   [Audio File Stream Services Reference](https://developer.apple.com/documentation/audiotoolbox/audio_file_stream_services)
*   [Apple Core Audio Format Specification 1.0](https://developer.apple.com/library/archive/documentation/MusicAudio/Reference/CAFSpec/CAF_intro/CAF_intro.html#//apple_ref/doc/uid/TP40001862) Apple’s Core Audio Format (CAF) 格式
*   **[Core Audio Utility Classes](https://developer.apple.com/library/archive/samplecode/CoreAudioUtilityClasses/Introduction/Intro.html) Apple的Core Audio中使用到的示例代码。**
*   WWDC中关于Audio的视频 [Audio](https://developer.apple.com/videos/media/audio)
*   [Audio Toolbox](https://developer.apple.com/documentation/audiotoolbox?language=objc)

*注意：现在已经没有CoreAudio SDK这个东西了，参考 [How to find the CoreAudio SDK?](https://developer.apple.com/forums/thread/17893)，倒是有两个Xcode tools：AU Lab和HALLab，可以通过Xcode的Open Developer Tool > More Developer Tools...下载关于AudioTools的文件，然后拷贝安装包里面的app到Xcode目录下的Applications中，重启Xcode就可以看到新安装的tools*

包含视频的资料：

*   [Media Playback Programming Guide](https://developer.apple.com/library/archive/documentation/AudioVideo/Conceptual/MediaPlaybackGuide/Contents/Resources/en.lproj/Introduction/Introduction.html)
*   [AVFoundation](https://developer.apple.com/documentation/avfoundation?language=objc)
*   [Core Video Programming Guide](https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/CoreVideo/CVProg_Intro/CVProg_Intro.html) 已停止更新

WWDC资料：

在 [Topics](https://developer.apple.com/videos/topics/) 中着Media下的[Audio](https://developer.apple.com/videos/media/audio)和[Video](https://developer.apple.com/videos/media/video)。

## 多线程 & 并发

*   [Grand Central Dispatch](https://apple.github.io/swift-corelibs-libdispatch/)
*   [Concurrency Programming Guide](https://developer.apple.com/library/archive/documentation/General/Conceptual/ConcurrencyProgrammingGuide/Introduction/Introduction.html)
*   [Threading Programming Guide](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/Multithreading/Introduction/Introduction.html)
*   [Blocks Programming Topics](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/Blocks/Articles/00_Introduction.html)

## Metal

## Networking

*   [Networking Overview](https://developer.apple.com/library/archive/documentation/NetworkingInternetWeb/Conceptual/NetworkingOverview/Introduction/Introduction.html)

## dynamic lib

*   [Dynamic Library Programming Topics](https://developer.apple.com/library/archive/documentation/DeveloperTools/Conceptual/DynamicLibraries/000-Introduction/Introduction.html)
*   [Mach-O Programming Topics](https://developer.apple.com/library/archive/documentation/DeveloperTools/Conceptual/MachOTopics/0-Introduction/introduction.html)

# GUI

*   [View Programming Guide](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/CocoaViewsGuide/Introduction/Introduction.html)

## Cocoa

*   [Cocoa Event Handling Guide](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/EventOverview/Introduction/Introduction.html)
*   [Cocoa Drawing Guide](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/CocoaDrawingGuide/Introduction/Introduction.html)
*   [Quartz 2D Programming Guide](https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/Introduction/Introduction.html) 也就是 Core Graphics

## 自动布局

*   [Auto Layout Guide](https://developer.apple.com/library/archive/documentation/UserExperience/Conceptual/AutolayoutPG/index.html)

## 各种控件

*   [SplitViews: Using NSSplitView in a variety of different ways](https://developer.apple.com/library/archive/samplecode/SplitViews/Introduction/Intro.html)
*   WWDC ：[View Based NSTableView Basic to Advanced](https://developer.apple.com/videos/play/wwdc2011/120/)

