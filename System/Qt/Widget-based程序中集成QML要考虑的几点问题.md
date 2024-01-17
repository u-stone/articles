[toc]

*本篇是为了技术分享准备的，更详细（也比较杂乱）的看另一篇 [QML知识点](http://note.youdao.com/noteshare?id=702fe243f4a63985096ecacd6fe8bd38)*

# Widget（C\++）中集成QML

对应场景：在已有的基于widget的Qt程序中集成QML

## 加载qml文件

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

##  Widget（C\++）调用QML对象方法

### 调用function

通过QMetaObject::invokeMethod方法直接调用QML中的function：

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

### 触发QML的signal

QML中的signal是一种特殊的属性。QML中定义的属性有on<属性>Changed的事件。

## Widget（C\++）读写QML对象属性

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

通过前面介绍的加载qml文件的方式加载之后，通过QQuickWidget/QQuickView的rootObject()得到qml中的根节点对象；然后通过查找得到某个命名的qml对象（子节点）。然后通过QQmlProperty, 或者 QObject::setProperty() and QObject::property()的方法操作qml对象的属性：
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

# QML中使用C++扩展

## 支持QML扩展的自定义C\++类

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

## QML中的signal触发，调用C\++方法

通过QObject::connect，把qml头节点的信号绑定到C\++方法。

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


# QML中调用QML、JS

## QML使用自定义QML模块

如果要在a.qml中使用自定义QML模块CustomModule.qml，仅需导入CustomModule.qml所在目录，如果两个文件在同一个目录甚至都不需要导入就可以直接使用，就像是注册类自定义QML模块一样。

```
// file: CustomModule.qml

Item {
    id: custom_module
    Rectangle {
        id: rect
        color: "red"
    }
}


// file: a.qml
// 如果CustomModule.qml在其他路径下，比如在相对目录../cutome_modules下，那么需要添加下面的导入语句：
// import "../cutome_modules"
Item {
    Rectangle {
        id: some_rect
        color: "green"
    }
    
    CustomModule {
        id: custome_item
        ...
    }
}
```

## QML中使用JS

除了直接在qml文件中直接定义js函数之外，可以将JS函数定义在一个.js文件中，然后import到使用该js文件的地方。

```
// file: click_event.js

function clickItem() {
    console.log("user click the item")
}

// file: a.qml
import "click_event.js"

Item {
    id: my_item
    width: 300
    height: 300
    
    MouseArea {
        id: user_op_area
        
        onClicked: {
            clickItem()
        }
    }
}

```

## QML的signal绑定QML的方法

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

## QML的signal绑定QML的signal

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
