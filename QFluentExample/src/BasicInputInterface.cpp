#include "BasicInputInterface.h"

BasicInputInterface::BasicInputInterface(QWidget *parent)
    : GalleryInterface("基本输入", "qfluentwidgets.components.widgets", parent)
{
    setObjectName("basicInputInterface");

    addExampleCard("带有文本的简单按钮", new PushButton("标准按钮", this), "");
}
