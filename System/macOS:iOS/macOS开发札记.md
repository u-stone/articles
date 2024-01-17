[toc]

# OC代码

## 分割oc代码

按照这种方式：

```
// MyClass+Networking.h
#import MyClass.h

@interface MyClass(Networking)

//method declarations here

@end
```

```
#import MyClass+Networking.h

@implementation MyClass(Networking)

//method definitions here

@end
```

## 单例
cocoa中各类控件
```c
@interface Kraken : NSObject
@end

@implementation Kraken

+ (instancetype)sharedInstance {
    static Kraken *sharedInstance = nil;
    static dispatch_once_t onceToken;

    dispatch_once(&onceToken, ^{
        sharedInstance = [[Kraken alloc] init];
    });
    return sharedInstance;
}

@end
```

参考 [THE RIGHT WAY TO WRITE A SINGLETON](https://krakendev.io/blog/the-right-way-to-write-a-singleton)

## block保存在NSDictionary中，以及取出

ref: [How do I make a C struct into a class property? [duplicate]](https://stackoverflow.com/questions/14881041/how-do-i-make-a-c-struct-into-a-class-property)

```cpp
// 保存
NSDictionary *blocks = [NSDictionary dictionaryWithObjectsAndKeys:
                              ^{NSLog(@"Log Block 1");}, @"Block1",
                              ^{NSLog(@"Log Block 2");}, @"Block2",
                              nil];

// 取出
void(^myAwesomeBlock)() = [blocks objectForKey:key];
myAwesomeBlock();
```

## C struct如何用作oc类的property

```
typedef struct s {
    int i;
} s;

@interface Test : NSObject {
    s *myS;
}
@property (nonatomic, assign) s *myS;
@end

@implementation Test
@synthesize myS;
- (id) init {
    self = [super init];
    myS = malloc(sizeof(s));
    myS->i = 0;
    return self;
}
@end


// somewhere later.
Test *t = [[Test alloc] init];
t.myS->i = 10;
```

## 可变参数函数的定义

ref: [How to create variable argument methods in Objective-C](https://stackoverflow.com/questions/4804674/how-to-create-variable-argument-methods-in-objective-c)

[Variable argument lists in Cocoa](https://www.cocoawithlove.com/2009/05/variable-argument-lists-in-cocoa.html)

```

// 声明
- (void)methodWithFormat:(NSString*)format, ...

// 定义
- (void)methodWithFormat:(NSString*)format, ... {
  va_list args;
  va_start(args,format);
  //loop, get every next arg by calling va_arg(args,<type>)
  // e.g. NSString *arg=va_arg(args,NSString*) or int arg=(args,int)
  va_end(args);
}

// 声明必须以nil结尾
+ (NSArray *)arrayWithObjects:(id)object, ... NS_REQUIRES_NIL_TERMINATION;

// 非NSString类型这样操作 (loop)
- (void)logMessage:(NSString *)format, ... {
  va_list args;
  va_start(args, format);

  id arg = nil;
  while ((arg = va_arg(args,id))) {
  /// Do your thing with arg here
  }

  va_end(args);
}
```

主要就是形式上：函数末尾加上[ **, ...** ]

## 如何传递可变参数

ref: [how to pass variable arguments to another method?](https://stackoverflow.com/questions/2391780/how-to-pass-variable-arguments-to-another-method)

```
..
[obj aMethod:@"test this %d parameter", 1337);
[obj anotherMethod:@"test that %d parameter", 666);
..

-(void) aMethod:(NSString *)a, ... 
{
    va_list ap;
    va_start(ap, a);

    [self anotherMethod:a withParameters:ap]; 

    va_end(ap);
}

-(void) anotherMethod:(NSString *)a, ...
{
    va_list ap;
    va_start(ap, a);

    [self anotherMethod:a withParameters:ap]; 

    va_end(ap);
}

-(void) anotherMethod:(NSString *)a withParameters:(va_list)valist 
{
    NSLog([[[NSString alloc] initWithFormat:a arguments:valist] autorelease]);
}
```

## 属性 vs 实例变量

OC支持实例变量和属性两种字段方式，但是属性自带getter&setter，实例变量则不然。如何使用看使用者。

参考：
- [Objective-c的@property 详解](https://www.cnblogs.com/zilongshanren/archive/2011/08/03/2125728.html)
- [Property, Instance Variable and @synthesize](http://hongchaozhang.github.io/blog/2015/12/19/propety-instance-variable-and-at-synthesize/)
- [When should I use @synthesize explicitly?](https://stackoverflow.com/questions/19784454/when-should-i-use-synthesize-explicitly)。

实例变量可以显式地设置访问权限：@private，@protected，@public。而且可以写在

```
@interface xxx {
@private
    int v1;
}
@end
```

也可以写在

```
@implement xxx {
@protected:
    int v2;
} 
@end

# 自定义视图变更约束

```cpp
- (void)updateConstraints {
    // add constrains.

    [super updateConstraints];
}

```

```
- (void)viewDidMoveToWindow
{
    if (_actionBtn == nil) {
        _actionBtn = [NSButton buttonWithTitleNERTC:_title target:_target action:_sel];
        [_actionBtn setBezelStyle:NSBezelStyleRoundRect];
        [self addSubview:_actionBtn];
    }
}
```

## 定时器block的使用

```
if (@available(macOS 10.12, *)) {
    NSTimer* timer = [NSTimer timerWithTimeInterval:1 repeats:NO block:^(NSTimer * _Nonnull timer) {
        weakSelf.sendBtn.enabled = YES;
        // ...
    }];
    NSRunLoop* loop = [NSRunLoop currentRunLoop];
    [loop addTimer:timer forMode:NSDefaultRunLoopMode];
    [loop runUntilDate:[NSDate dateWithTimeIntervalSinceNow: 1]];
} else {

}
```

## 枚举所有进程

参考 [Technical Q&A QA1123 Getting List of All Processes on Mac OS X](https://developer.apple.com/library/archive/qa/qa2001/qa1123.html)，以及开源代码：[ProcInfo](https://github.com/objective-see/ProcInfo)

## 通过一个文件地址获取文件icon

```
NSImage *image = [[NSWorkspace sharedWorkspace] iconForFile:path];

// 通过bundleid获取文件路径
path = [[NSWorkspace sharedWorkspace] absolutePathForAppBundleWithIdentifier:bundleIdentifier];
```

## 通过pid获取bundle id

[How to find Bundle Identifier from known PID?](https://stackoverflow.com/questions/3309681/how-to-find-bundle-identifier-from-known-pid)



## 激活一个app窗口

```c
// from webrtc.
bool WindowCapturerMac::FocusOnSelectedSource() {
  if (!window_id_)
    return false;

  CGWindowID ids[1];
  ids[0] = window_id_;
  CFArrayRef window_id_array =
      CFArrayCreate(nullptr, reinterpret_cast<const void**>(&ids), 1, nullptr);

  CFArrayRef window_array =
      CGWindowListCreateDescriptionFromArray(window_id_array);
  if (!window_array || 0 == CFArrayGetCount(window_array)) {
    // Could not find the window. It might have been closed.
    RTC_LOG(LS_INFO) << "Window not found";
    CFRelease(window_id_array);
    return false;
  }

  CFDictionaryRef window = reinterpret_cast<CFDictionaryRef>(
      CFArrayGetValueAtIndex(window_array, 0));
  CFNumberRef pid_ref = reinterpret_cast<CFNumberRef>(
      CFDictionaryGetValue(window, kCGWindowOwnerPID));

  int pid;
  CFNumberGetValue(pid_ref, kCFNumberIntType, &pid);

  // TODO(jiayl): this will bring the process main window to the front. We
  // should find a way to bring only the window to the front.
  bool result =
      [[NSRunningApplication runningApplicationWithProcessIdentifier: pid]
          activateWithOptions: NSApplicationActivateIgnoringOtherApps];

  CFRelease(window_id_array);
  CFRelease(window_array);
  return result;
}

// from : https://github.com/gbammc/Thor/blob/1.5.6/Thor/ShortcutMonitor.swift
// 把最小化的app设为激活状态
static func register() {
    let apps = AppsManager.manager.selectedApps
    for app in apps where app.shortcut != nil {
        MASShortcutMonitor.shared().register(app.shortcut, withAction: {
            guard defaults[.EnableShortcut] else { return }

            if let frontmostAppIdentifier = NSWorkspace.shared.frontmostApplication?.bundleIdentifier,
                let targetAppIdentifier = Bundle(url: app.appBundleURL)?.bundleIdentifier,
                frontmostAppIdentifier == targetAppIdentifier {
                NSRunningApplication.runningApplications(withBundleIdentifier: frontmostAppIdentifier).first?.hide()
            } else {
                if #available(macOS 10.15, *) {
                    let configuration = NSWorkspace.OpenConfiguration()
                    configuration.activates = true
                    NSWorkspace.shared.openApplication(at: app.appBundleURL,
                                                       configuration: configuration) { _, error in
                        if let error = error {
                            NSLog("ERROR: \(error)")
                        }
                    }
                } else {
                    NSWorkspace.shared.launchApplication(app.appName)
                }
            }
        })
    }
}
```

类似的问题：[Activate a window using its Window ID](https://stackoverflow.com/questions/47152551/activate-a-window-using-its-window-id)，[SWIFT restore program from dock in mac os](https://stackoverflow.com/questions/60435888/swift-restore-program-from-dock-in-mac-os)

但是上面的代码并不能将窗口从最小化状态恢复为激活状态。

## 如何通过pid得到进程的path

[`URLForApplicationWithBundleIdentifier` ](https://developer.apple.com/documentation/appkit/nsworkspace/1534053-urlforapplicationwithbundleident)，可以获取到app的URL，但是如何获取bundle id呢？



# C/C++有关的问题

## 找不到<new> file

在target"Build Phases" -> "Link Binary with Libraries"中添加 libc++.tbd，然后在"Build Settings" -> "Apple Clang - Language - C++" -> "C++ Standard Library" 设置为 libc++

参考：[XCode 10 - "new" file not found](https://developer.apple.com/forums/thread/108537)

## 如何从编译后的文件中获取原始符号名称

C/C++编译器编译的时候会修改函数名，这个动作称作 mangling，如果想要获取函数，变量的原始名称，可以通过编译器提供的方法：

- macOS使用了clang或者gcc，可以使用：[abi::__cxa_demangle](https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a01696.html)
- msvc提供了一个工具：[undname](https://docs.microsoft.com/en-us/cpp/build/reference/decorated-names?view=msvc-160)，dbghelper中有一个函数用于翻译符号：[UnDecorateSymbolName](https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-undecoratesymbolname)

这里还有一个在线的网址，提供名称翻译：https://demangler.com

还有一个不错的资料汇总：[C++ Name Mangling/Demangling](http://www.kegel.com/mangle.html)

# macOS上的开源软件

[open-source-mac-os-apps](https://github.com/serhii-londar/open-source-mac-os-apps)

# 常用类以及常用方法

## NSDate

```
// 打印当前时间
NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
[formatter setDateFormat:@"yyyy-MM-dd'T'HH:mm:ss.SSS"];
//Optionally for time zone conversions
//[formatter setTimeZone:[NSTimeZone timeZoneWithName:@"..."]];
NSString *stringFromDate = [formatter stringFromDate:myNSDateInstance];


// 打印指定的一个时间
long long time = [self getDateTimeTOMilliSeconds:[NSDate date]];
NSLog(@"%llu",time);
NSDate *dat = [self getDateTimeFromMilliSeconds:time];
NSDateFormatter * formatter = [[NSDateFormatter alloc ] init];
[formatter setDateFormat:@"yyyy-MM-dd hh:mm:ss.SSS"];
NSString *date = [formatter stringFromDate:dat];
NSString *timeLocal = [[NSString alloc] initWithFormat:@"%@", date];
```

## CGImage

CGImage旋转

```
-(CGImageRef) rotate90Degree:(CGImageRef) cgImageRef 
{
    int cgImageWidth = (int)CGImageGetWidth(cgImageRef);
    int cgImageHeight = (int)CGImageGetHeight(cgImageRef);
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    NSUInteger bytesPerPixel = 4;
    
    int targetWidth = cgImageHeight;
    int targetHeight = cgImageWidth;
    
    NSUInteger bytesPerRow = bytesPerPixel * targetWidth;
    NSUInteger bitsPerComponent = 8;

    CGContextRef context = CGBitmapContextCreate(nil, targetWidth, targetHeight,
                                                 bitsPerComponent, bytesPerRow, colorSpace,
                                                 kCGImageAlphaPremultipliedLast | kCGBitmapByteOrderDefault);
                                                 
    CGContextRotateCTM (context, -M_PI_2);
    CGContextTranslateCTM(context, -(int)targetHeight, 0);      
    
    CGContextDrawImage(context, CGRectMake(0, 0, cgImageWidth, cgImageHeight), cgImageRef);
    
    CGImageRef rotatedCGImage = CGBitmapContextCreateImage(context);
    return rotatedCGImage;
}
```

ref: [josephsieh/CGImage Rotation](https://gist.github.com/josephsieh/19b2f180b7559c3405a1292b15080477)

# cocoa中各类控件

## NSViewController控制的NSView的生命周期

NSViewController的方法中有如下调用顺序：

```
View life cycle
1. viewDidLoad
2. viewWillAppear
3. viewDidAppear
    User interaction cycle:
    1. updateViewConstraints
    2. viewWillLayout
    3. viewDidLayout
4. viewWillDisappear
5. viewDidDisappear
```

其中User interaction cycle是指用户交互操作过程中的调用逻辑。


## NSSplitView自动布局

- [demitri/SplitViewAutoLayout](https://github.com/demitri/SplitViewAutoLayout)

[如何让NSSplitView的子View的大小固定](https://blog.csdn.net/Koupoo/article/details/6755991)


## NSScrollView

设置背景为透明：

```
[textScroll setDrawsBackground:NO];
```

## NSButton的各种style

- [A guide to NSButton styles](https://mackuba.eu/2014/10/06/a-guide-to-nsbutton-styles/)
- [自定义NSButton实现hover、highlighted效果](https://www.jianshu.com/p/bf24d0e43eda)

### 如何设置radio button的group（状态互斥）

方法是将多个ratio style button的action设置为同一个函数，然后通过输入参数（sender）来判断当前点击操作是哪一个，进而设置On/Off状态。比如下面有两个button：btn1和btn2，更新状态的方法是：

```
- (IBAction)onClickChannelType:(NSButton *)sender {
    [btn1 setState: sender == btn1 ? NSControlStateValueOn : NSControlStateValueOff];
    [btn2 setState: sender == btn2 ? NSControlStateValueOn : NSControlStateValueOff];
}
```

## NSWindow

NSWindow 有一个比较复杂的behavior属性：

- NSWindowCollectionBehaviorDefault：默认行为
- NSWindowCollectionBehaviorCanJoinAllSpaces: 窗口出现在所有Space中。无论活动空间如何，菜单栏都会显示此窗口的标题。Dock 不会为此窗口显示指示器。
- NSWindowCollectionBehaviorMoveToActiveSpace：窗口随用户移动到活动空间。跟上面的 NSWindowCollectionBehaviorCanJoinAllSpaces 不可以一同使用，会冲突并且报错。
- NSWindowCollectionBehaviorManaged：窗口参与Space和Exposé的管理，默认行为
- NSWindowCollectionBehaviorTransient: 窗口浮动在Space中，当用户切换到另一个应用程序时会隐藏。对于浮动调色板窗口或其他辅助窗口，应使用此设置，以便在其父窗口可见时保持屏幕上可见。不会在 Expose 中显示。
- NSWindowCollectionBehaviorStationary: 窗口不受Exposé影响；它保持可见和静止，就像桌面窗口一样。如果对高亮功能的窗口设置这个行为，那么在 Expose 显示全部窗口时，二者也是叠加在一起的。
- NSWindowCollectionBehaviorParticipatesInCycle: 如后面所述，这里的Cycle是指 "Cycle Through Windows"
- NSWindowCollectionBehaviorIgnoresCycle: 跟上面一条是一组两个相反的属性，当窗口具有NSWindowCollectionBehaviorIgnoresCycle行为时，这意味着当用户使用“Cycle Through Windows (循环浏览窗口)”命令（通常是Command-`（反引号）或Command-~（波浪符））循环浏览窗口时，该窗口不会被包括在内。这对于在用户浏览其他窗口时应保持原位的窗口非常有用，例如工具面板或信息面板。
- NSWindowCollectionBehaviorFullScreenPrimary：当窗口具有 NSWindowCollectionBehaviorFullScreenPrimary 行为时，这意味着当用户为应用程序进入全屏模式时，该窗口可以成为全屏模式下显示的主窗口。在多窗口应用程序中，只应有一个窗口具有此行为。如果有多个窗口具有此行为，则系统会选择其中一个窗口作为全屏窗口。这对于指定应用程序中哪个窗口应在用户进入全屏模式时显示非常有用。
- NSWindowCollectionBehaviorFullScreenAuxiliary：窗口是一个辅助窗口，可以与全屏的primary窗口一起显示。
- NSWindowCollectionBehaviorFullScreenNone：表示窗口不支持全屏，用户点击了最大化按钮将不会发生任何事情。
- NSWindowCollectionBehaviorFullScreenAllowsTiling： 当窗口具有这种行为时，它意味着该窗口可以在全屏模式下平铺到屏幕的一侧。这是在macOS Catalina（10.15）中引入的一项功能，允许两个应用程序在全屏模式下平等地共享屏幕，类似于iOS上的“分屏视图”功能。
- NSWindowCollectionBehaviorFullScreenDisallowsTiling：当一个窗口具有这种行为时，这意味着该窗口无法在全屏模式下与屏幕的一侧平铺。这对于在全屏模式下始终应占据整个屏幕的窗口非常有用，例如视频播放器或游戏。


### 什么是 Exposé 
Exposé是苹果（Apple）操作系统macOS中的一项功能，旨在帮助用户管理和快速切换多个打开的窗口和应用程序。

Exposé可以通过按下特定的快捷键或手势来激活，具体取决于您的系统设置。一旦激活，Exposé将以动画形式将所有打开的窗口缩小并排列在屏幕上，使您能够一目了然地查看和访问它们。这样，您就可以快速切换到所需的窗口或应用程序，而无需手动逐个查找或最小化窗口。

Exposé还提供了其他功能，如显示桌面、显示窗口的缩略图等，以帮助用户更好地组织和管理多个窗口。

需要注意的是，Exposé功能在不同版本的macOS中可能有所不同，因此具体的操作方式和功能可能会有所区别。您可以在系统偏好设置中的“Mission Control”或“触控板”设置中找到有关Exposé的详细信息和配置选项。

也就是三指上滑出现的预览图界面。

### Cycle Through Windows (循环浏览窗口) 命令是什么

"Cycle Through Windows"命令是指在操作系统中切换活动窗口的快捷键组合。不同的操作系统和桌面环境可能有不同的快捷键组合来实现此功能。

在Windows操作系统中，"Cycle Through Windows"命令通常使用Alt+Tab键来实现。按住Alt键并连续按下Tab键，你可以在打开的窗口之间进行切换。每次按下Tab键，活动窗口会切换到下一个窗口。当你松开Alt键时，选中的窗口将成为活动窗口。

在Mac OS操作系统中，"Cycle Through Windows"命令使用Command+Tab键来实现。按住Command键并连续按下Tab键，你可以在打开的应用程序之间进行切换。每次按下Tab键，活动窗口会切换到下一个应用程序。当你松开Command键时，选中的应用程序将成为活动窗口。

这个命令对于快速切换活动窗口或应用程序非常有用，可以提高你的工作效率。

## NSTableView相关

使用IB工具创建的tableview，想要只保留一列，结果一直会出现两列：解决方案，[How can I have the only column of my NSTableView take all the width of the TableView?](https://stackoverflow.com/questions/7545490/how-can-i-have-the-only-column-of-my-nstableview-take-all-the-width-of-the-table)，这是IB的一个bug，自己设置好只有一列之后，自行拖动column view到table view大小就没问题了。

table view的点击事件：[NSTableView: detecting a mouse click together with the row and column](https://stackoverflow.com/questions/18560509/nstableview-detecting-a-mouse-click-together-with-the-row-and-column)
 
[Self-sizing Table View Cells](https://www.raywenderlich.com/8549-self-sizing-table-view-cells)
 
## sheet窗口

[How to Modally Present Windows as Sheets in OS X (Yosemite & Mavericks)](https://www.nickkuh.com/mac-os-x/how-to-modally-present-windows-as-sheets-in-os-x-yosemite-mavericks/2015/01/)

使用NSWindow的`beginSheet:completionHandler:`方法弹出sheet窗口，需要注意两点：

1. 必须要在调用的地方保存弹出NSWindowController的变量，不然会失败。
2. 必须在弹出的窗口中设置取消 `Visible at launch`

## NSTextField

设置只能输入数字：

[Restrict NSTextField input to numeric only? NSNumberformatter](https://stackoverflow.com/questions/4652689/restrict-nstextfield-input-to-numeric-only-nsnumberformatter)

方法是subclass NSNumberFormatter，然后重载函数：

```
@interface OnlyNumberFormatter : NSNumberFormatter
@end

@implementation OnlyNumberFormatter

- (BOOL)isPartialStringValid:(NSString *)partialString
            newEditingString:(NSString **)newString
            errorDescription:(NSString **)error {
    // Make sure we clear newString and error to ensure old values aren't being
    // used
    if (newString) {
        *newString = nil;
    }
    if (error) {
        *error = nil;
    }

    static NSCharacterSet *nonDecimalCharacters = nil;
    if (nonDecimalCharacters == nil) {
        nonDecimalCharacters =
        [[NSCharacterSet decimalDigitCharacterSet] invertedSet];
    }

    if ([partialString length] == 0) {
        return YES;
        // The empty string is okay (the user might just be deleting
        // everything and starting over)
    } else if ([partialString rangeOfCharacterFromSet:nonDecimalCharacters]
               .location != NSNotFound) {
        return NO; // Non-decimal characters aren't cool!
    }

    return YES;
}

@end

```

然后把这个formatter设置给NSTextField对象：

```
    OnlyNumberFormatter* formatter = [[OnlyNumberFormatter alloc] init];
    [_sendRate setFormatter:formatter];

```


## NSApperance

addSubView的时候会回调子view的 `- (void)viewDidChangeEffectiveAppearance` 方法

```
#0	0x000000010008f543 in -[NRTCLeftActionCustomeAudioView viewDidChangeEffectiveAppearance] at /Volumes/WorkSpace/lava/sdk/demo/mac/NERTCDemo/NERTCDemo Mac/UI/MainView/LeftArea/CellView/NRTCLeftActionCustomeAudioView.m:116
#1	0x00007fff332df3b6 in -[NSView _viewDidChangeAppearance:] ()
#2	0x00007fff332dd09f in -[NSView _setSuperview:] ()
#3	0x00007fff332dc8c4 in -[NSView addSubview:] ()
```

## MTKView

有三种渲染模式。

如果要保存MTKView中的数据为图片，可以这样：

```c
- (IBAction)onCapture:(id)sender
{
    id<MTLTexture> lastDrawableDisplayed = [metalView.currentDrawable texture];

    int width = (int)[lastDrawableDisplayed width];
    int height = (int)[lastDrawableDisplayed height];
    int rowBytes = width * 4;
    int selfturesize = width * height * 4;

    void *p = malloc(selfturesize);

    [lastDrawableDisplayed getBytes:p bytesPerRow:rowBytes fromRegion:MTLRegionMake2D(0, 0, width, height) mipmapLevel:0];

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrder32Little | kCGImageAlphaFirst;

    CGDataProviderRef provider = CGDataProviderCreateWithData(nil, p, selfturesize, nil);
    CGImageRef cgImageRef = CGImageCreate(width, height, 8, 32, rowBytes, colorSpace, bitmapInfo, provider, nil, true, (CGColorRenderingIntent)kCGRenderingIntentDefault);

    UIImage *getImage = [UIImage imageWithCGImage:cgImageRef];
    CFRelease(cgImageRef);
    free(p);

    NSData *pngData = UIImagePNGRepresentation(getImage);
    UIImage *pngImage = [UIImage imageWithData:pngData];
    UIImageWriteToSavedPhotosAlbum(pngImage, self,
        @selector(completeSavedImage:didFinishSavingWithError:contextInfo:), nil);
}
```

不过，需要设置 `metalView.framebufferOnly = false;` ，不然的话 `getBytes` 方法会报错：failed assertion ‘texture must not be a framebufferOnly texture.’。不过这样一来，性能会有损耗，参考：[framebufferOnly](https://developer.apple.com/documentation/metalkit/mtkview/1535998-framebufferonly?language=objc)中的：

> If the value is NO, you can sample or perform read/write operations on the textures, but at a cost to performance.


ref: [How can I make screen shot image from MTKView in iOS?](https://stackoverflow.com/questions/35115605/how-can-i-make-screen-shot-image-from-mtkview-in-ios).

或者： [Is there a way to capture Metal output as a bitmap or other format suitable for printing?](https://stackoverflow.com/questions/60900412/is-there-a-way-to-capture-metal-output-as-a-bitmap-or-other-format-suitable-for)

## NSTrackArea

在自定义NSView中添加鼠标track区域。

```c
- (void)viewDidMoveToWindow {
    [super viewDidMoveToWindow];

    self.roomIDTextFiled.editable = YES;
    self.roomIDTextFiled.selectable = YES;

    NSTrackingArea *trackingArea = [[NSTrackingArea alloc]
                                    initWithRect:[self bounds]
                                    options:(NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways)
                                    owner:self
                                    userInfo:nil];
    [self addTrackingArea:trackingArea];
}

- (void)mouseDown:(NSEvent *)event {
    [super mouseDown:event];

}

- (void) mouseEntered:(NSEvent*)theEvent {
}

- (void) mouseExited:(NSEvent*)theEvent {
}
```

## NSTextView

字体镂空效果：[Drawing attributed strings that are both filled and stroked](https://developer.apple.com/library/archive/qa/qa1531/_index.html)

# 调试

Mac上好像不能简单地dump memory：[How to dump physical memory on OS X?](https://superuser.com/questions/1314635/how-to-dump-physical-memory-on-os-x)，但是可以通过启用Mac上的mem设备来实现：[How to access /dev/mem in OSX?](https://apple.stackexchange.com/questions/114319/how-to-access-dev-mem-in-osx)

[WINDOWS, MAC OSX AND LINUX MEMORY DUMP HOW TO](https://holdmybeersecurity.com/2016/06/12/linux-and-windows-memory-dump/)

**注意：模拟器上无法使用Mac的摄像头和麦克风，必须到真机上跑app**

## Xcode有时候调试程序会卡死

[Main thread locks during debugging](https://developer.apple.com/forums/thread/113167)

就是Xcode中有一个选项会记录GCD的跨调度异步调用堆栈，但是有时候会带来问题，所以可以在Scheme里面关掉：

Product > Scheme > Edit Scheme > Run > Options Queue Debugging > “Enable backtrace recording”

另外一个关于Xcode卡死检测的资料：[Main Thread Checker in XCode](https://medium.com/@trivediniki94/main-thread-checker-in-xcode-8b9f3f8ce10)，打开Main Thread Checker可以在主线程卡死的时候，停到造成卡死的那一行代码上。

## 崩溃捕获

参考开源代码：
- Google的 [Crashpad](https://chromium.googlesource.com/crashpad/crashpad/+/HEAD/doc/developing.md)
- [microsoft/plcrashreporter](https://github.com/microsoft/plcrashreporter)
- [kstenerud/KSCrash](https://github.com/kstenerud/KSCrash)

## 打印运行状态的堆栈

ref： [Print the name of the calling function to the debug log](https://stackoverflow.com/questions/4046833/print-the-name-of-the-calling-function-to-the-debug-log)

```
#include <execinfo.h>

void *addr[2];
int nframes = backtrace(addr, sizeof(addr)/sizeof(*addr));
if (nframes > 1) {
    char **syms = backtrace_symbols(addr, nframes);
    NSLog(@"%s: caller: %s", __func__, syms[1]);
    free(syms);
} else {
    NSLog(@"%s: *** Failed to generate backtrace.", __func__);
}
```

或者使用方法：

```
 NSLog(@"%@",[NSThread callStackSymbols]);
```

# 音视频

- 博客 [码农人生](http://msching.github.io) 里面有一个介绍音频的系列博文

# 关闭app不会调用applicationWillTerminate

[applicationWillTerminate not being called](https://developer.apple.com/forums/thread/126418)

xcode 11之后添加了一个新的plist项：`Application can be killed immediately when user is shutting down or logging out`，把参数设置为NO，或者删掉就可以调用到了。

# Xcode相关

# CPU架构

macOS上有两种：
- x86_64：基于Intel x64架构的机器时间上大约在2005~2021
- arm64：2020年之后Apple出的基于Apple Silicon的机器

iOS和iPadOS
- arm64:iPhone 5s以及之后的机器，iPad air，Air2和pro，基于A7及之后的芯片
- armv7s：A6和A6X芯片，用于iPhone5，iPhone 5C，iPad 4.
- armv7:32位的arm芯片，用于A5及之前的芯片。

参考：[CPU Architectures](https://docs.elementscompiler.com/Platforms/Cocoa/CpuArchitectures/)

# 系统相关

## Enabling DTrace on macOS Sierra

- Reboot into Recovery Mode (hold down ⌘R during boot)
- Launch a shell and run `csrutil enable --without dtrace`
- Reboot and allow machine to boot normally

也可以使用 `csrutil disable` 完全禁用安全性。

## DTrace on Sierra
- [DTrace on Sierra](https://craftware.xyz/tips/DTrace.html)