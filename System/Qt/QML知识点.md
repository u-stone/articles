[toc]

# QML

## QML是什么

QML是一种声明性语言，它允许根据用户界面的可视组件以及它们之间的交互和关联来描述用户界面。

## Qt QML是什么

Qt QML模块为使用QML语言开发应用程序和库提供了一个框架。它定义并实现了语言和引擎基础结构，并提供了一个API，使应用程序开发人员能够将QML语言扩展为自定义类型，并将QML代码与JavaScript和C\++集成在一起。QT QML模块既提供QML API又提供C\++ API。

Qt QML模块提供了语言，框架。还有很多可视化的组件、模型视图支持，动画框架，以及很多可视化用户接口。

## Qt Quick是什么

可以认为：Qt Quick模块是QML应用程序的标准库。

## Qt Quick Compiler

Qt Quick Compiler是Qt Quick应用程序的一个开发附加组件，它允许您将QML源代码编译成最终的二进制文件。使用此加载项时，应用程序的启动时间显著缩短，不再需要将.qml文件与应用程序一起部署。

自Qt 5.11之后，Qt Quick Compiler的功能已经集成到Qt Quick模块中。这个单独的附加组件在Qt早期的长期支持版本中仍然可用。

## QML engine是什么

QQmlEngine类提供了一个用于实例化QML组件的环境。

## [Qt Quick] vs [Qt Quick 2]

Qt Quick Controls 在Qt 5.1中引入；使用QPainter之流绘制。

Qt Quick Controls 2在Qt 5.7中引入；基于Scene Graph并使用OpenGL绘制。

## QML可否像动态语言一样运行时加载执行

可以。不过程序中添加的qml文件，如果不添加到qrc中，是无法加载成功的，因为qml的加载要么是qrc中的资源，要么是一个本地的绝对路径下的文件。

## qbs是什么

Qbs是一种构建自动化工具，旨在方便地管理跨多个平台的软件项目的构建过程。

## qmlscene是什么

qmlscene是用来测试qml文件的工具，即使程序不完整也可以。使用的命令：

```
qmlscene myqmlfile.qml
```

可以通过-I custom-qml-path的方式添加自定义qml类型。

参考 [Prototyping with qmlscene](https://doc.qt.io/qt-5.9/qtquick-qmlscene.html)

在Qt 4.x中对应的工具是qmlviewer。


## qss和qml

实际上，qml中不支持qss。最多可以使用 [Qml Styling](https://wiki.qt.io/Qml_Styling) 的方式。不过，qml控件定义不同的样式也很方便，而且支持state所以不支持也没有大碍；Quick控件是支持rich text格式文本的。

## Qt Quick UI Forms(.ui.qml file) and QML(.qml file)

Qt Quick UI Forms可以使用可视化工具编辑，QML文件不行，但是可以使用qmlscene来辅助实现预览。Qt Creator中的Tools -> External -> Qt Quick -> Qt Quick 2 Preview (qmlscene)就是这个工具对当前文件的预览调用。

不过，也可以使用.ui.qml的可视化编辑工具打开qml文件来编辑，方法是打开一个.ui.qml文件，然后在可视化编辑工具的导航中，选择“工程”，找到qml文件，双击打开即可。

使用Qt Quick UI Forms的话，可以使用PhotoShop/GIMP导出为QML文件格式，免去开发写界面的麻烦，参考：[Exporting Designs from Graphics Software](https://doc.qt.io/qtcreator/quick-export-to-qml.html)

### .ui.qml/.qml使用场景

Qt Quick UI Forms支持的是QML语言的一个子集，它目前：

在特性上：
- 不支持JavaScript blocks
- 不支持纯表达式之外的绑定
- 不支持信号处理函数
- 不支持除根item之外的item的状态
- 不支持非继承自QQuickItem和Item的根items

在类型上：
- 不支持Behavior
- 不支持Binding
- 不支持Cavas
- 不支持Component
- 不支持Shader Effect
- 不支持Timer
- 不支持Transform
- 不支持Transition

那么除了使用上面的地方，可以在仅有UI没有逻辑的地方使用Qt Quick UI Forms。（感觉这样的地方不是很多，所以这块儿没有完全弄清楚）

## 基于QWidget的UI程序如何集成Qt Quick [2]

对应场景：QWidget中需要使用QML文件做视图。

[Integrating QML Code with Existing Qt UI Code](https://doc.qt.io/archives/qt-4.8/qml-integration.html) - 介绍说，Qt Quick 可以通过QDeclarativeView加载qml，但是目前Qt 5.9中已经没有这个类了。所以这个选项排除。

[Deploying QML Applications](https://doc.qt.io/archives/qt-5.5/qtquick-deployment.html) 介绍了两个方法在C++中加载QML：

- 一种是使用QQuickView，这个类最终继承自QWindow，所以创建成功之后可以嵌入QWindow结构中，比如说作为主窗口放在main函数中：

    ```c
    QQuickView view;
    view.setSource(QUrl::fromLocalFile("application.qml"));
    view.show();
    ```

- 第二种是使用QQmlEngine，使用的场景是：

    1. 不想使用QQuickView
    2. qml中没有任何可视化控件
    
    ```c
    QQmlEngine engine;
    QQmlContext *objectContext = new QQmlContext(engine.rootContext());

    QQmlComponent component(&engine, "application.qml");
    QObject *object = component.create(objectContext);  
    ```

- 除了上面那篇文章提到的情况。在一般QWidget中，可以使用使用QQuickWidget，这个类最终继承自QWidget，所以创建成功之后可以嵌入QWidget结构中：
    ```c
    QQuickWidget *view = new QQuickWidget();
    view->setSource(QUrl(QStringLiteral("test_view.qml")));
    
    // then, add into widget architecture
    QBoxLayout *layout = new QHBoxLayout();
    ui->centralWidget->setLayout(layout);
    layout->addWidget(view);
    ```
    要注意，上面的setSource中参数是运行时的地址，如果是设置了相对地址，需要把资源设置为相对于.app的地址，或者是把qml资源添加到qrc中，使用qrc:/的方式加载。
    
创建好js文件之后，同qml文件一样，也必须在qrc中添加了，才能在qml文件中import成功。

### 读/写加载的qml文件中对象的属性

ref: [Interacting with QML Objects from C++](https://doc-snapshots.qt.io/qt5-5.9/qtqml-cppintegration-interactqmlfromcpp.html)

要读写qml中对象的属性，首先需要qml对象有一个名字，方便查找到，比如下面例子中的"rect"：

```
import QtQuick 2.0

Item {
    width: 100; height: 100

    Rectangle {
        anchors.fill: parent
        objectName: "rect"
    }
}
```

通过前面介绍的加载qml文件的方式加载之后，通过QQuickWidget/QQuickView的rootObject()得到qml中的根节点对象；然后通过查找得到某个命名的qml对象（子节点）：
```c
QQuickView view;
view.setSource(QUrl::fromLocalFile("MyItem.qml"));
view.show();
QObject *object = view.rootObject();

// 1. 直接操作根节点
object->setProperty("width", 500);
QQmlProperty(object, "width").write(500);

// 2. 查找到非根节点并操作
QObject *rect = object->findChild<QObject*>("rect");
if (rect) {
    rect->setProperty("color", "red");
}
```

#### 操作属性的方法

可以通过QQmlProperty, 或者 QObject::setProperty() and QObject::property()的方法操作qml对象的属性：

```c
// MyItem.qml
import QtQuick 2.0

Item {
    property int someNumber: 100
}
```

```c
QQmlEngine engine;
QQmlComponent component(&engine, "MyItem.qml");
QObject *object = component.create();

qDebug() << "Property value:" << QQmlProperty::read(object, "someNumber").toInt();
QQmlProperty::write(object, "someNumber", 5000);

qDebug() << "Property value:" << object->property("someNumber").toInt();
object->setProperty("someNumber", 100);
```

#### 操作QML方法

```c
// MyItem.qml
import QtQuick 2.0

Item {
    function myQmlFunction(msg) {
        console.log("Got message:", msg)
        return "some return value"
    }
}

// main.cpp
QQmlEngine engine;
QQmlComponent component(&engine, "MyItem.qml");
QObject *object = component.create();

QVariant returnedValue;
QVariant msg = "Hello from C++";
QMetaObject::invokeMethod(object, "myQmlFunction",
        Q_RETURN_ARG(QVariant, returnedValue),
        Q_ARG(QVariant, msg));

qDebug() << "QML function returned:" << returnedValue.toString();
delete object;
```

#### 信号处理

```c
// MyItem.qml
import QtQuick 2.0

Item {
    id: item
    width: 100; height: 100

    signal qmlSignal(string msg)

    MouseArea {
        anchors.fill: parent
        onClicked: item.qmlSignal("Hello from QML")
    }
}

// main.cpp
class MyClass : public QObject
{
    Q_OBJECT
public slots:
    void cppSlot(const QString &msg) {
        qDebug() << "Called the C++ slot with message:" << msg;
    }
};

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QQuickView view(QUrl::fromLocalFile("MyItem.qml"));
    QObject *item = view.rootObject();

    MyClass myClass;
    QObject::connect(item, SIGNAL(qmlSignal(QString)),
                     &myClass, SLOT(cppSlot(QString)));

    view.show();
    return app.exec();
}
```

#### QML中信号与方法的连接

参考下面的例子，信号与方法的连接

```
Rectangle {
    id: relay

    signal messageReceived(string person, string notice)

    Component.onCompleted: {
        relay.messageReceived.connect(sendToPost)
        relay.messageReceived.connect(sendToTelegraph)
        relay.messageReceived.connect(sendToEmail)
        relay.messageReceived("Tom", "Happy Birthday")
    }

    function sendToPost(person, notice) {
        console.log("Sending to post: " + person + ", " + notice)
    }
    function sendToTelegraph(person, notice) {
        console.log("Sending to telegraph: " + person + ", " + notice)
    }
    function sendToEmail(person, notice) {
        console.log("Sending to email: " + person + ", " + notice)
    }
    function removeTelegraphSignal() {
        relay.messageReceived.disconnect(sendToTelegraph)
    }
}
```

信号与信号的连接

```
Rectangle {
    id: forwarder
    width: 100; height: 100

    signal send()
    onSend: console.log("Send clicked")

    MouseArea {
        id: mousearea
        anchors.fill: parent
        onClicked: console.log("MouseArea clicked")
    }

    Component.onCompleted: {
        mousearea.clicked.connect(send)
    }
}
```


## 自定义QML模块 

### 普通QML Module

一个.qml文件，要求文件名首字母大写，保存位置位于使用该qml中定义控件的qml文件的同目录，而且不需要使用import的方式导入。这种方式用起来简便，但是不能为其他目录的qml文件提供支持，要想达到这个目标需要使用下面提到的Identified Module。

### Identified Module -- 可理解为预先定义好的模块 

ref: [Identified Modules](https://doc.qt.io/qt-5.9/qtqml-modules-identifiedmodules.html)

不过想要其他模块使用到还需要做一些工作。

这类自定义的QML模块需要安装在QML的导入目录才能被QML引擎找到，比并且需要使用import语句导入。

导入目录概念和设置参考：[QML Import Path](https://doc.qt.io/qt-5.9/qtqml-syntax-imports.html#qml-import-path)

#### 创建和安装设置

首先要明确一点，如果要定义Identified Module，那么这个module就不属于你创建的工程，而是属于QML引擎使用的全局的模块，所以对应的代码也不需要定义在你创建的工程中，放了也没关系。

假定，将要被编写的module存放目录在ROOT_PATH下，module的名字叫CustomQmlModule，那么目录结构如下：

```
ROOT_PATH
   |-- CustomQmlModule
   |-- ...
```

那么需要在文件夹CustomQmlModule下面创建一个qmldir文件，以及其他功能文件（比如，.qml，C/C++代码，js代码、qml中用到的图片资源等）。假定其他文件有：MyQmlCtrl.qml、MyQmlCtrlList.qml、其他文件比如图片资源，也就是说文件目录如下：

```c
CustomQmlModule
   |-- Recommend
         |-- RecommendCtrl.qml
         |-- RecommendList.qml
         |-- 其他文件
         |-- qmldir
```

其中qmldir的内容是：

```c
module Recommend

RecommendCtrl 1.0 RecommendCtrl.qml
RecommendList 1.0 RecommendList.qml

typeinfo pulgin.qmltypes

```

写好这些文件的功能之后，运行下面的命令创建qmltype文件：

```
qmlplugindump CustomQmlModule.Recommend 1.0 \
    ROOT_PATH > ROOT_PATH/CustomQmlModule/Recommend/plugin.qmltypes
```

这样就可以生成一个QML模块 - CustomQmlModule.Recommend，该模块中有两个控件RecommendCtrl和RecommendList。

做完上面的操作之后，想要QML引擎找到这个QML模块，还需要将这个模块所在路径添加到QML的导入路径，方法有几种：
- 环境变量 QML2_IMPORT_PATH 中添加ROOT_PATH目录(即：自定义模块所在目录)。如果将ROOT_PATH添加到环境变量，那么使用该模块的地方写法是:import CustomQmlModule.Recommend 1.0；如果将ROOT_PATH/CustomQmlModule添加到环境变量，那么导入写法就是：import Recommend 1.0
- 【推荐】资源qrc:/qt-project.org/imports中添加文件，这里/qt-project.org/imports就是qrc中的一个前缀，所有资源文件都可以直接添加到这个前缀下面。
- 将这个CustomQmlModule文件夹放在Qt Creator的[QLibraryInfo::Qml2ImportsPath](https://doc.qt.io/qt-5.9/qlibraryinfo.html#LibraryLocation-enum)目录下，比如我的Mac电脑上是/Users/hj/Qt5.9.7/5.9.7/clang_64/qml目录下，即：
/Users/hj/Qt5.9.7/5.9.7/clang_64/qml/CustomQmlModule/Recommend，不过这种方法影响范围较大，不是十分底层的模块封装不建议使用。
- 【个人实践建议】如果是在调试+使用QML模块的阶段，建议将QML模块通过添加到资源qrc:/qt-project.org/imports的方式，让Qt Creator可以便捷地打开编辑，并且在Qt Creator的项目设置中添加一个自定义qmlplugindump编译步骤，并放在第一步；这样的话，就可以在每次修改了/qt-project.org/imports下面的资源之后，及时地编译、注册模块。在模块稳定之后，可以就去掉自定义编译步骤了。

如果是想要在运行时加载qml文件时，设置引用的自定义/第三方QML模块，可以通过调用QQmlEngine::addImportPath()或者设置环境变量QML2_IMPORT_PATH来实现。 

一旦添加到QML导入目录，那么这个模块就不需要添加到具体某个项目中了。这里个人推荐使用环境变量QML2_IMPORT_PATH的方式，可以将所有的自定义模块定义在一个特定的目录，或者通过一个git项目管理，然后各个开发通过git的方式下载到某个目录ROOT_PATH，并把这个ROOT_PATH添加到QML2_IMPORT_PATH。这样只要设置好环境变量之后，更新模块仅需更新该目录中的文件、或者git pull即可。

#### 远程的QML模块

将远端的QML模块URL地址设置到QML的导入路径，这样也可以用上面相同的方式使用。

不过，通过网络加载的QML模块只能有qml和js文件，不能有C/C++插件。

#### 导入路径错误调试

使用环境变量 QML_IMPORT_TRACE，具体方法是在Qt Creator的工程设置tab中的Build Envirenment中添加QML_IMPORT_TRACE，并且不需要设置值。

#### 版本管理

QML有版本更新的用法。这种用法提供了一个好处，可以通过版本更新添加新的功能，而老的功能可以继续保留在老的代码中。一个模块中可以有多种文件：.qml，.qmltypes，C/C++代码，js代码等。都可以保存在qmldir文件中。

如果要更新版本，可以将上面例子中的Recommend文件夹名修改为Recommend2.0，表示CustomQmlModule中只有这个Recommend模块需要更新。也可以将CustomQmlModule文件夹名修改为CustomQmlModule2.0表示整个模块都要升级了。当然记得保留原来的目录，因为老的代码中还是用这些老的模块。

## 如何在QML中集成<自定义>Widget/QObject

这中场景对应：QML中使用C\++对象类型，下面的方法介绍如何将Qt C\++对象的属性、方法等暴露给QML类型系统（QML Type System）。

ref: [Exposing Attributes of C++ Types to QML](https://doc-snapshots.qt.io/qt5-5.9/qtqml-cppintegration-exposecppattributes.html)

基于QML engine与Qt meta-object系统的整合，QML可以（比较）简便地使用C++代码，只要继承自QObject即可。所以继承自QObject的类中可以供QML直接访问使用的成员有：
- 属性 Q_PROPERTY()
    ```
    class Message : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString author READ author WRITE setAuthor NOTIFY authorChanged)
    public:
        void setAuthor(const QString &a) {
            if (a != m_author) {
                m_author = a;
                emit authorChanged();
            }
        }
        QString author() const {
            return m_author;
        }
    signals:
        void authorChanged();
    private:
        QString m_author;
    };
    ```
- 方法（public类型的slot，由Q_INVOKABLE修饰的方法）
    ```c
    // main.cpp
    class ApplicationData : public QObject
    {
        Q_OBJECT
    public:
        Q_INVOKABLE QDateTime getCurrentDateTime() const {
            return QDateTime::currentDateTime();
        }
    };
    
    int main(int argc, char *argv[]) {
        QGuiApplication app(argc, argv);
    
        QQuickView view;
    
        ApplicationData data;
        view.rootContext()->setContextProperty("applicationData", &data);
    
        view.setSource(QUrl::fromLocalFile("MyItem.qml"));
        view.show();
    
        return app.exec();
    }
    
    // MyItem.qml
    import QtQuick 2.0
    
    Text {
        text: applicationData.getCurrentDateTime()
      
        // 使用Connections检测Qt C++对象的属性变化
        Connections {
            target: applicationData
            onDataChanged: console.log("The application data changed!")
        }
    }
    ```
- 信号 包括属性中指定的Notify信号
- 由Q_ENUMS定义的枚举类型

*命名建议： 属性变化的Notify建议命名格式：<property>Changed，QML engine无论如何都是使用on<Property>Changed的格式来为属性变化的信号处理函数命名*

如果要使用列表类型的属性，不能使用QList<T>，因为QList<T>不是继承自QObject的类型；而是需要使用QQmlListProperty，同时，QQmlListProperty的模版类型需要注册以供QML类型系统识别。

~~在使用QQmlContext的过程中发现：目前仅能把C\++数据暴露给qml中的根上下文，如果是非根节点的qml对象，无法使用C\++数据。如果这个属实的话，如果一个qml文件中的 **非根节点** qml类型对象使用到C\++数据，那么就需要单独把这块儿qml代码拎出来，重新写到一个新的根节点中包含该C\++数据的qml文件中。~~

~~后来~~发现可以通过为非根节点指定objectName，并在C\++代码中通过QObject的findChild的方法找到子节点对象，进而设置给它C\++数据。

## 布局：[Item Positioners] vs [Qt Quick Layouts]

在 [Important Concepts In Qt Quick - Positioning](https://doc.qt.io/qt-5/qtquick-positioning-topic.html) 中介绍到，一般情况下，使用Positioners就可以高效地完成布局，同时它们也是自己的容器；而Layouts除了控制位置，还可以控制大小，适用于界面大小变化的场景。

## Model-View-Delegate

对应场景：QML中需要使用C\++对象作为数据源。当然，最终这个qml文件也可以供 Widget-based / QuickView-based / QML 的程序使用。

ref: [Models and Views in Qt Quick](https://doc.qt.io/qt-5.9/qtquick-modelviewsdata-modelview.html)

![在这里插入图片描述](https://img-blog.csdnimg.cn/20191128203950787.png)

如上图所示：  
**Model**：数据源  
**View**：展示数据的容器  
**Delegate**：决定数据如何在View中展示 

Qt Quick中提供的View有ListView，GridView，PathView。

在View中可以绑定Model和Delegate，一般情况下使用匿名Component来做Delegate
- 常规的Model-View-Delegate模型中：
  - 绑定了Delegate之后，可以在Delegate中访问View，以及View中绑定的Model，比如下面例子中的Component（作为Delegate使用）中的fruit可以直接访问ListView.view来得到ListView对象，进而访问ListView中的属性、以及model，进一步可以访问model中的属性(language)
  - 绑定（View中绑定了model以及Delegate）之后，Delegate就可以直接访问Model中的（自定义）成员，QML中称这种自定义成员为 **role**（下面例子中的name，cost）。
  - 如果Delegate中的属性与Model中的属性出现命名冲突，可以使用model.<named_property>。
  - Delegate中使用model中的数据，还有一些预设的属性，比如model中数据的序号：index。当这个item被删除之后，index会变成-1。
  - 可以通过model的id来动态修改数据
    
```
常规代码示例 :

Rectangle {
     width: 200; height: 200

    ListModel {
        id: fruitModel
        property string language: "en"
        ListElement {
            name: "Apple"
            cost: 2.45
        }
        ListElement {
            name: "Orange"
            cost: 3.25
        }
        ListElement {
            name: "Banana"
            cost: 1.95
        }
    }

    Component {
        id: fruitDelegate
        Row {
                id: fruit
                Text { text: " Fruit: " + name; color: fruit.ListView.view.fruit_color }
                Text { text: " Cost: $" + cost }
                Text { text: " Language: " + fruit.ListView.view.model.language }
        }
    }

    ListView {
        property color fruit_color: "green"
        model: fruitModel
        delegate: fruitDelegate
        anchors.fill: parent
    }
}
```
- 可以通过XmlListModel提供XML类型数据，具体参考 [XmlListModel QML Type](https://doc.qt.io/qt-5.9/qml-qtquick-xmllistmodel-xmllistmodel.html)
- 一个特殊的Model：Object Model，不需要在绑定这种Model的View中指定Delegate，因为它包含的就是可视化的Item。参考 [ObjectModel QML Type](https://doc.qt.io/qt-5.9/qml-qtqml-models-objectmodel.html)
- 还可以使用一个整数作为model放在view中。不过这个时候这个model没有任何role。
- 使用一个QML Object作为model
- 使用C\++ Data做Model [Using C++ Models with Qt Quick Views](https://doc.qt.io/qt-5.9/qtquick-modelviewsdata-cppmodels.html)  
  可以使用QStringList，QVariantList，QObjectList作为数据源，但是这种方法无法直接处理数据改变的情况，只能调用setContextProperty()来重新设置数据。这里有一个小的注意事项：如果需要QQuickView/QQuickWidget来加载qml并展示数据的话，需要在view的setSource之前，view取得context并调用setContextProperty，不然的话，运行虽然没有问题，但是会有一个警告说找不到这个property。
  ```
  QML代码：
  ListView {
    width: 100; height: 100

    model: myModel
    delegate: Rectangle {
        height: 25
        width: 100
        Text { text: modelData }
    }
  }
  
  C++代码：
  QStringList dataList;
  dataList.append("Item 1");
  dataList.append("Item 2");
  dataList.append("Item 3");
  dataList.append("Item 4");

  QQuickView view;
  QQmlContext *ctxt = view.rootContext();
  ctxt->setContextProperty("myModel", QVariant::fromValue(dataList));
  ```
  不过，上面这种方法通过rootContext设置属性，无法通过QQuickWidget加载数据。
- 使用QAbstractItemModel作为基类，这种方法给了一种复杂，当然也灵活的设置数据源的方法。具体参考示例代码abstractitemmodel, 也可以参考 [Model Subclassing Reference](https://doc.qt.io/qt-5.9/model-view-programming.html#model-subclassing-reference)。使用这种方法的时候，注意重载roleNames函数返回的数据QHash<int, QByteArray>，第二个参数就是对应的可以在qml文件中直接使用的属性值，这个命名要规范。
```
QHash<int, QByteArray> RecommendListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    // role names will be use in qml files.
    roles[Role_CategoryName] = "categoryName";
    roles[Role_ContentId] = "contentID";
    roles[Role_ContentName] = "contentName";
    roles[Role_CoverUrl] = "coverUrl";
    roles[Role_CreatorName] = "creatorName";
    roles[Role_IsTry] = "isTry";
    roles[Role_LinkUrl] = "linkUrl";
    roles[Role_ShowCount] = "showCount";
    roles[Role_CountType] = "countType";
    roles[Role_IsCoupon] = "isCoupon";
    roles[Role_OrigianlPrice] = "originalPrice";
    roles[Role_CurrentPrice] = "currentPrice";
    roles[Role_ActivityName] = "activityName";
    roles[Role_HasDaKa] = "hasDaKa";
    roles[Role_Tag] = "tag";
    roles[Role_ActivityType] = "activityType";
    return roles;
}
```
- 支持SQL数据模型，具体参考：[Using the SQL Model Classes](https://doc.qt.io/qt-5.9/sql-model.html) 可以在Qt Creator中搜索"sql"查找示例代码

## 为QML写C\++插件

ref: [Writing QML Extensions with C++](https://doc.qt.io/qt-5.9/qtqml-tutorials-extending-qml-example.html)，[Creating C++ Plugins for QML](https://doc.qt.io/qt-5.9/qtqml-modules-cppplugins.html)

如果需要为QML类型提供一种独立lib文件格式的插件，可以按照下面的步骤：
- 创建pro工程
- 创建自定义类，这些类需要继承自QQmlExtensionPlugin，并且用Q_PLUGIN_METADATA()注册自定义C\++类；重载registerTypes()，并用qmlRegisterType()注册要导出的类
- 添加qmldir文件
- 运行时设置import path。

## 其他

- **使用signal** - 信号处理函数名字格式：on<Signal> ，<Signal>是信号的名字，首字母要大写。
- C++对象想要在qml中使用，需要继承自QObject
- QML的属性命名规则：必须小写字母开头；C++的类可以通过Q_PROPERTY定义属性，由QML类型系统注册；反之，QML也可以定义属性
- 自定义QML对象，可以供其他qml文件使用。但是这类文件的命名要求由字母、数字、下划线的组合，并且要首字母要大写。
- QML中的js：属性绑定，信号处理函数，自定义函数，引入的js文件(import "factorial.js" as MathFunctions)，连接信号到js函数
- Item（继承自QtObject，而QtObject继承自QObject）是Qt Quick中所有可视化对象的基础类型，Rectangle（继承自Item）是可视化对象的基类。Item是一个比较复杂的对象，最好先阅读一下具体介绍 [Item QML Type](https://doc.qt.io/qt-5.9/qml-qtquick-item.html)。
- 设置item位置的方法：设置x、y，使用anchors，使用布局，使用Column/Row
- 布局有4种：行RowLayout、列ColumnLayout、网格GridLayout、栈StackLayout
- 注册在QML中使用的C++类需要在QQmlApplicationEngine对象声明之前，使用qmlRegisterType()或者其他接口。
- 可以使用QQuickStyle::setStyle("Material")接口设置Qt Quick Controls 2的样式。ref: [Styling Qt Quick Controls 2](https://doc.qt.io/qt-5.9/qtquickcontrols2-styles.html)，也可以在.app运行的时候设置stle参数，或者加一个特殊的配置文件 ":/qtquickcontrols2.conf"，具体 ref: [Qt Quick Controls 2 Configuration File](https://doc.qt.io/qt-5.9/qtquickcontrols2-configuration.html)。
- 使用Qt Quick Controls 2的话，下面的方式包含：
    - qml文件中：import QtQuick.Controls 2.2
    - C++类中：#include <QtQuickControls2>，并在pro/pri中添加QT += quickcontrols2
- 使用Qt Quick Controls的话，下面的方式包含：
    - qml文件中：import QtQuick.Controls 1.2
    - C++类中：#include <QtQuickControls>，并在pro/pri中添加QT += quickcontrols
- Qt Quick Templates是Qt Quick Controls 2的基础。为可视化控件提供接口。
- 可以使用WebEngineView 和 WebView来加载URL或者HTML字符串，但是后者是使用一个可用的本地web view，多用在移动平台。
- 属性定义：[default] property <propertyType> <propertyName>[ : <propertyValue>]，其中<propertyType>可参考：[All QML Basic Types](https://doc.qt.io/qt-5.9/qmlbasictypes.html)，[QML Basic Types](https://doc.qt.io/qt-5.9/qtqml-typesystem-basictypes.html)，以及C++与QML类型对应关系 [Data Type Conversion Between QML and C++](https://doc.qt.io/archives/qt-5.5/qtqml-cppintegration-data.html)
- 使用QML自带的控件用来做复杂的数据展示：树控件，表格，分隔视图等，参考Qt Quick Controls 的 [Navigation and Views](https://doc.qt.io/qt-5.9/qtquickcontrols-index.html#navigation-and-views)

# Qt Creator中自带的示例代码

contactlist：演示自定义C\++类做model，view是一个自定义Qt Quick UI Form类。

# 参考

**[D.0]** 主要入口
  - [Qt QML](https://doc.qt.io/qt-5.9/qtqml-index.html)
  - [All QML Modules](https://doc.qt.io/qt-5.9/modules-qml.html)
  - [All QML Types](https://doc.qt.io/qt-5.9/qmltypes.html)
  - [Qt Quick QML Types](https://doc.qt.io/qt-5.9/qtquick-qmlmodule.html)
  - [QML Applications](https://doc.qt.io/qt-5.9/qmlapplications.html)
  - [The QML Reference](https://doc.qt.io/qt-5.9/qmlreference.html)
  - Qt的枚举类型、全局函数列表：[Qt QML Type](https://doc-snapshots.qt.io/qt5-5.9/qml-qtqml-qt.html)
  - QML对象属性语法：[QML Object Attributes](https://doc.qt.io/qt-5.9/qtqml-syntax-objectattributes.html) (提到如何定义attached attribute)，[Property Binding](https://doc.qt.io/qt-5.9/qtqml-syntax-propertybinding.html)
  - 模块导入语法：[Import Statements](https://doc.qt.io/qt-5.9/qtqml-syntax-imports.html)
  - QML图形特效类型： [Qt Graphical Effect QML Types](https://doc.qt.io/qt-5.9/qtgraphicaleffects-qmlmodule.html)
  - 信号事件处理系统：[Signal and Handler Event System](https://doc.qt.io/qt-5.9/qtqml-syntax-signals.html)
  - MDN文档：[JavaScript](https://developer.mozilla.org/en-US/docs/Web/JavaScript) 既然QML支持javaScript，那就得看一下这个。

[D.1] 示例代码
- [Qt Examples And Tutorials](https://doc.qt.io/qt-5.9/qtexamplesandtutorials.html)
- 学习Qt Quick编程最好的开始与指导：**[Getting Started Programming with Qt Quick](https://doc.qt.io/qt-5.9/gettingstartedqml.html)**
- [Qt Quick Examples and Tutorials](https://doc.qt.io/qt-5.9/qtquick-codesamples.html)
- [Getting Started with Qt Quick Controls 2](https://doc.qt.io/qt-5.9/qtquickcontrols2-gettingstarted.html)  

[D.2] 另一种工程构建工具：Qbs：[Qbs Manual](https://doc.qt.io/qbs/index.html)  
[D.3] [Qt Quick Controls Overview](https://doc.qt.io/qt-5.9/qtquickcontrols-overview.html) 有两个版本： 
  - [Qt Quick Controls 2](https://doc.qt.io/qt-5.9/qtquickcontrols2-index.html)(introduced in Qt 5.7)  
  - [Qt Quick Controls](https://doc.qt.io/qt-5.9/qtquickcontrols-index.html)(introduced in Qt 5.1)

  关于使用哪一个版本，参考：  
  
  - [Getting Started with Qt Quick Controls 2](https://doc.qt.io/qt-5.9/qtquickcontrols2-gettingstarted.html)
  - 以及迁移指南：[Differences between Qt Quick Controls](https://doc.qt.io/qt-5.9/qtquickcontrols2-differences.html)  

[D.4] 杂项：
- 调试：[Debugging QML Applications](https://doc.qt.io/qt-5.9/qtquick-debugging.html)
- QML 编码规范: [QML Coding Conventions](https://doc.qt.io/qt-5.9/qml-codingconventions.html)
- qml预览：[Prototyping with qmlscene](https://doc.qt.io/qt-5.9/qtquick-qmlscene.html)
- 布局：[Item Positioners](https://doc.qt.io/qt-5.9/qtquick-positioning-layouts.html)，[Qt Quick Layouts](https://doc.qt.io/qt-5.9/qtquicklayouts-index.html)，[Positioning with Anchors](https://doc.qt.io/qt-5/qtquick-positioning-anchors.html)，[Right-to-left User Interfaces](https://doc.qt.io/qt-5/qtquick-positioning-righttoleft.html)
- 动画：[Important Concepts in Qt Quick - States, Transitions and Animations](https://doc.qt.io/qt-5.9/qtquick-statesanimations-topic.html)，[Qt Quick States](https://doc.qt.io/qt-5/qtquick-statesanimations-states.html)
- 性能建议：[Performance Considerations And Suggestions](https://doc.qt.io/qt-5.9/qtquick-performance.html)
- 3D: [Introducing Qt Quick 3D: A high-level 3D API for Qt Quick](https://www.qt.io/blog/2019/08/14/introducing-qt-quick-3d-high-level-3d-api-qt-quick)，Qt 5.9到5.13中叫 [Qt 3D](https://doc.qt.io/qt-5.9/qt3d-index.html)，Qt 5.14中叫[Qt Quick 3D](https://doc-snapshots.qt.io/qt5-5.14/qtquick3d-index.html)
- 可展示动态数据：[Models and Views in Qt Quick](https://doc.qt.io/qt-5/qtquick-modelviewsdata-modelview.html)
- 将Qt Quick工程添加到pro中：[Converting UI Projects to Applications](https://doc.qt.io/qtcreator/quick-converting-ui-projects.html)
- [Glossary Of QML Terms](https://doc.qt.io/qt-5.9/qml-glossary.html) QML中的概念， [Loader QML Type 懒加载](https://doc.qt.io/qt-5.9/qml-qtquick-loader.html)
- DPI相关：[Scalability](https://doc.qt.io/qt-5.9/scalability.html)   
- 控件的样式：[Qt Quick Controls Styles QML Types](https://doc.qt.io/qt-5.9/qtquick-controls-styles-qmlmodule.html)
- Qt Quick国际化：[Internationalization and Localization with Qt Quick](https://doc.qt.io/qt-5.9/qtquick-internationalization.html)
- QML命名规则：[Scope and Naming Resolution](https://doc.qt.io/qt-5.9/qtqml-documents-scope.html)
- OpenGL特效：[ShaderEffect](https://doc.qt.io/qt-5.9/qml-qtquick-shadereffect.html)
- QML性能工具：[Profiling QML Applications](https://doc.qt.io/qtcreator/creator-qml-performance-monitor.html)，[Qt Quick Scene Graph Default Renderer](https://doc.qt.io/qt-5/qtquick-visualcanvas-scenegraph-renderer.html)，一个PPT [Using The QML Profiler](https://www.kdab.com/wp-content/uploads/stories/slides/Day2/UlfHermann_qmlprofiler_0.pdf)

[D.5] 自定义QML对象
- 各种自定义QML类型示例：[Customizing Qt Quick Controls 2](https://doc-snapshots.qt.io/qt5-5.9/qtquickcontrols2-customize.html)
- QML模块定义文件：[Module Definition qmldir Files](https://doc.qt.io/qt-5/qtqml-modules-qmldir.html)
- [Defining Object Types through QML Documents](https://doc.qt.io/qt-5.9/qtqml-documents-definetypes.html)（定义在一个qml文件中）
- 也可以使用一个匿名的Component定义一个本地使用的QML对象，参考QML Object Types中的 [Defining Anonymous Types with Component](https://doc.qt.io/qt-5.9/qtqml-typesystem-objecttypes.html#defining-anonymous-types-with-component)
- [Qml Styling](https://wiki.qt.io/Qml_Styling)
- [Using QML Modules with Plugins](https://doc.qt.io/qtcreator/creator-qml-modules-with-plugins.html)
- [Identified Modules](https://doc.qt.io/qt-5.9/qtqml-modules-identifiedmodules.html)

[D.6] Qt Quick file (.qml) vs Qt Quick UI Forms file (.ui.qml)
- [Qt Quick](https://doc.qt.io/qt-5.9/qtquick-index.html)
- [Qt Quick UI Forms](https://doc.qt.io/qtcreator/creator-quick-ui-forms.html)  

[D.7] QML和C\++混编 :
- [Integrating QML and C++](https://doc.qt.io/qt-5.9/qtqml-cppintegration-topic.html)，[Interacting with QML Objects from C++](https://doc-snapshots.qt.io/qt5-5.9/qtqml-cppintegration-interactqmlfromcpp.html)
- [Defining QML Types from C++](https://doc.qt.io/qt-5.9/qtqml-cppintegration-definetypes.html)，[Exposing Attributes of C++ Types to QML](https://doc-snapshots.qt.io/qt5-5.9/qtqml-cppintegration-exposecppattributes.html)
- [Defining Object Types through QML Documents](https://doc.qt.io/qt-5.9/qtqml-documents-definetypes.html)
- 用C\++为QML写扩展（插件 lib类型）[Writing QML Extensions with C++](https://doc.qt.io/qt-5.9/qtqml-tutorials-extending-qml-example.html)，[Creating C++ Plugins for QML](https://doc.qt.io/qt-5.9/qtqml-modules-cppplugins.html)
- 通过js创建QML对象：[Dynamic QML Object Creation from JavaScript](https://doc.qt.io/qt-5.9/qtqml-javascript-dynamicobjectcreation.html)
- 在基于 QWidget 和 QGraphicsView 的程序中嵌入QML代码 [Integrating QML Code with Existing Qt UI Code](https://doc.qt.io/archives/qt-4.8/qml-integration.html)
- 还有 [Qt Quick C++ Classes](https://doc.qt.io/qt-5.9/qtquick-module.html) Qt Quick模块为C\++中嵌入QML提供的类。  
- Model-View模型：[Using C++ Models with Qt Quick Views](https://doc.qt.io/qt-5.9/qtquick-modelviewsdata-cppmodels.html)
- 将程序迁移到Qt 5：[Porting QML Applications to Qt 5](https://doc.qt.io/qt-5.9/qtquick-porting-qt5.html)
- C\++类型与QML类型的映射：[Data Type Conversion Between QML and C++](https://doc.qt.io/archives/qt-5.5/qtqml-cppintegration-data.html) 这篇中还介绍了C++对象在传给QML的时候所有权的归属问题。

[D.9] **一些概念** 
- 父控件：[Concepts - Visual Parent in Qt Quick](https://doc.qt.io/qt-5.9/qtquick-visualcanvas-visualparent.html) Z序值小于0会位于父控件后面，Z序值只会与兄弟控件和父控件做比对。
- **场景图**：[Qt Quick Scene Graph](https://doc.qt.io/qt-5.9/qtquick-visualcanvas-scenegraph.html)，Qt Quick 2中引入，基于OpenGL ES 2.0或OpenGL 2.0来渲染。Scene Graph是由QQuickWindow管理和渲染的。在许多平台上，Scene Graph运行在专门的线程（非GUI线程）中。
- 坐标系： [Concepts - Visual Coordinates in Qt Quick](https://doc.qt.io/qt-5.9/qtquick-visualcanvas-coordinates.html)  
 
[D.10] js相关: 
- [Defining JavaScript Resources in QML](https://doc.qt.io/qt-5.12/qtqml-javascript-resources.html)
- [Importing JavaScript Resources in QML](https://doc.qt.io/qt-5.9/qtqml-javascript-imports.html)  
- [JavaScript Expressions in QML Documents](https://doc.qt.io/qt-5.9/qtqml-javascript-expressions.html) 
- [List of JavaScript Objects and Functions](https://doc.qt.io/qt-5.9/qtqml-javascript-functionlist.html)

[D.11] 非官方资料:
- [Qt开发学习教程](https://blog.51cto.com/9291927/2138876) 中一篇不错的介绍C++和QML交互的文章[QT开发（六十九）——QML与C++混合编程](https://blog.51cto.com/9291927/1975383)  
- [Qt Widgets、QML、Qt Quick 的区别](https://blog.csdn.net/liang19890820/article/details/54141552)
- 电子书：[A Book about Qt5](https://qmlbook.github.io/index.html)