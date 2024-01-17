1， Text放在一个BorderImage里面，但是需要根据文本的内容调整宽度，可以这么通过childrenRect来动态计算设置，参考[Stretching element to contain all children](https://stackoverflow.com/questions/6212445/stretching-element-to-contain-all-children)：

```
BorderImage {
    id: imageDiscount
    height: 14
    width: childrenRect.width + 5

    border.bottom: 3
    border.top: 3
    border.right: 3
    border.left: 8

    source: "./res/activity_type.png"

    Text {
        id: text3
        color: "#ff3271"
        text: activityName
        anchors.centerIn: parent
        leftPadding: 8
        rightPadding: 3
        font.pixelSize: 10
    }
}
```

2，创建一个带圆角的Image，ref: [Image rounded corners in QML](https://stackoverflow.com/questions/6090740/image-rounded-corners-in-qml)，Image不支持圆角设置，不过可以设置layer.effect为OpacityMask，然后创建一个蒙层。

```
Image {
    id: img
    layer.enabled: true
    layer.effect: OpacityMask {
        maskSource: Item {
            width: img.width
            height: img.height
            Rectangle {
                anchors.centerIn: parent
                anchors.fill: parent
                radius: 5
            }
        }
    }
}
```

3，使用Model-View模型建立视图的时候，如果出现了滚动鼠标触发item移出可见区域，然后移回来的时候发现item视图消失了，这个时候要看一下，是不是设置给view的model的C\++对象被释放了，或者是不是使用了临时变量。ref: Data Type Conversion Between QML and C++ 之 [Data Ownership](https://doc.qt.io/qt-5/qtqml-cppintegration-data.html#data-ownership)，和[Qml Listview items disappear when scrolling](https://stackoverflow.com/questions/42100029/qml-listview-items-disappear-when-scrolling/42134712)，一般情况下，所有权属于C\++，但是如果在qml文件中显式调用C\++函数来得到对象的话，这个所有权默认归QML引擎，想要明确所有权的话，可以使用 QQmlEngine::setObjectOwnership() 指定 QQmlEngine::CppOwnership 参数来设置为C\++对象所有。

4，创建一个带圆角的按钮：
```
Button {
    height: 36
    width: text_content.width + 36
    anchors.centerIn: parent

    background: Rectangle {
        id: btn_got_know
        height: 36
        width: text_content.width + 36
        anchors.verticalCenter: parent.verticalCenter
        color: "#2F5AFF"
        radius: 18
    }

    Text {
        id: text_content
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#FFFFFF"
        font.pixelSize: 14
        text: qsTr("Get to know")
    }
}
```



```
graph TB
Control-->AbstractButton
AbstractButton-->Button
```
如上派生关系图，Button继承自Control，Control中有一个background属性，类型是Item，所以可以使用Rectange来实现圆角。ref: [Control QML Type](https://doc.qt.io/qt-5.9/qml-qtquick-controls2-control.html)

5, 使用Qt Creator 5.9.6打包的时候有一个问题：需要添加qmldir参数，而且，不能使用always-overwrite参数。参考[[REG 5.6.1->5.6.2] macdeploy -always-overwrite fails on qml/quick plugins](https://bugreports.qt.io/browse/QTBUG-57265)。所以需要去掉-always-overwrite。

6, 如果代码中混合使用了QML和Widget，那么由于QtCreator默认是开启两种调试器的，所以会出现一种情况，在调试C\++代码的时候，老是提示你加载QML调试器，解决办法是在Project->Build & Run->Run->Debugger Setting中，取消选中Enable QML。

