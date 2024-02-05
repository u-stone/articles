
# 代码信息

分支：29.0.0

代码位于 `obs-studio\plugins\win-dshow`

# 分析

**从 UI 操作流程上说：**

1. 打开obs之后，新建一个视频采集设备： obs项目层面的入口代码位于 `libobs\obs-source.c` 中的 `obs_source_create_internal()`)，调用到dshow设备的创建是 `plugins\win-dshow\win-dshow.cpp` 中的静态函数 `CreateDShowInput`。
    *OBS中使用一个设计模式，各个plugin模块在加载的时候注册一些统一的回调，然后在需要使用的时候，统一通过UI操作触发这些设置的函数指针来执行对应的操作。*
    *具体到 `win-dshow` 模块，这个模块加载的时候，通过 `plugins\win-dshow\win-dshow.cpp` 中的 `RegisterDShowSource` 函数，注册给了 `obs_source_info`，这时候由 `info->create` 函数指针调用下来。*
2. 然后激活设备(代码`obs-studio\plugins\win-dshow\win-dshow.cpp` 中的 `DShowInput::DShowLoop`)，会触发事件：
    - 激活：`Action::Activate` 对应主要操作 `DShowInput::Activate`，这个是所有操作里面最复杂的。
    - 取消激活： `Action::Deactivate`，对应 `DShowInput::Deactivate`

3. 激活的过程就是dshow构建pipeline的过程。


**从实际运行角度说：**

代表采集的 `DShowInput` 对象包含一个 `DShow::Device` 对象。`Device` 的 UI 交互操作由 `DShowInput` 通过  `DShowInput::DShowLoop` 来驱动。

整个视频采集的核心就是 `obs-studio\plugins\win-dshow\libdshowcapture\source\dshowcapture.cpp` 中的 `DShow::Device` 类。这个类主要负责音视频设备的设置，`UI` 对话框交互， `dshow` 中 `Graph` 的构建，以及开始和停止操作。

而这个 `DShow::Device` 内部又有一个 `DShow::HDevice` 作为 `context` 来完成具体操作。

这个 `DShow::HDevice` 类负责具体实现，比如：

- `CreateGraph`
- `SetupVideoCapture`, `SetupAudioCapture`
- `SetVideoConfig`, `SetAudioConfig`
- `ConnectFilters`, `DisconnectFilters`，以及`ConnectPins`
- `RenderFilters`

这里的实现，对应激活过程 `DShowInput::Activate` 中的3步：

- `ResetGraph`
- `UpdateVideoConfig`，`UpdateAudioConfig`
- `ConnectFilters`

总体来说调用层级就是：

```
DShowInput
     |--> DShow::Device
                   |--> DShow::HDevice 主要实现在这里
```

**单说激活设备的流程：**

1. 首先是重建一个graph：创建好 `graph`, `capture builder`, `media control`，并做好初始化
2. 然后枚举设备，并获取当前选择的设备信息
3. 设置当前设备参数：根据选择的设备path和name，获取 `CLSID_VideoInputDeviceCategory` 类型的采集设备filter，然后获取到这个filter上的 `PIN_CATEGORY_CAPTURE` 类型IPin，然后从中Query一个`IAMStreamConfig`，并设置 `AM_MEDIA_TYPE`。而后添加这个设置好的 filter 到graph中。





# 其他

## Blackmagic Design
DeckLink Video是Blackmagic Design公司推出的一系列专业视频捕捉卡和播放卡的产品线。这些卡可以用于电影、电视、广播和流媒体等领域，提供高质量的视频捕捉和播放功能。DeckLink Video卡支持各种视频格式和分辨率，并且具有低延迟和高带宽的特点，适用于需要高性能视频处理的应用场景。

Blackmagic Design是一家总部位于澳大利亚的跨国公司，专注于视频和音频技术的研发和制造。该公司成立于1984年，并于2002年在美国成立了总部。Blackmagic Design在视频和电影制作领域具有很高的声誉，他们的产品被广泛应用于电影、电视、广播、流媒体和其他行业。

Blackmagic Design不断推出创新的产品和解决方案，包括视频捕捉卡、视频转换器、视频监控设备、视频编辑软件等。他们的产品以高质量、高性能和易于使用而著称，并且价格相对较为亲民，深受专业和非专业用户的喜爱。

Blackmagic Design在全球范围内拥有广泛的用户群体，并与许多知名的影视制作公司和广播公司建立了合作关系。他们的产品也多次获得了行业内的认可和奖项，成为许多专业人士的首选。

总的来说，Blackmagic Design在视频和音频技术领域的发展非常积极和成功，为行业带来了许多创新和进步。

## Elgato
Elgato是一家专注于视频内容创作和游戏流媒体领域的公司。他们开发和制造各种高质量的视频和音频捕捉设备、视频转换器、视频录制软件等产品，为用户提供了丰富的工具和解决方案来创建和分享优质的视频内容。

Elgato的产品系列包括Game Capture系列、Stream Deck系列和Cam Link系列等。Game Capture系列是他们最著名的产品之一，主要用于游戏内容创作，允许玩家录制游戏画面、实时直播和分享游戏内容。Stream Deck系列是一款可编程的触控面板，可用于简化流媒体和视频制作过程中的操作，提高工作效率。Cam Link系列是一款视频转换器，将相机的HDMI输出转换为USB视频信号，使用户能够使用相机进行高质量的直播和录制。

除了硬件产品，Elgato还开发了一系列软件来辅助视频内容创作，例如Game Capture软件和Stream Deck软件。这些软件提供了直观易用的界面，让用户能够轻松控制和管理他们的设备，并进行视频编辑、实时流媒体和社交媒体分享等操作。

Elgato的产品和解决方案已经得到了广大用户和专业人士的认可和喜爱。他们致力于为用户提供高质量、易于使用和创新的视频内容创作工具，帮助他们实现更好的创作效果和体验。