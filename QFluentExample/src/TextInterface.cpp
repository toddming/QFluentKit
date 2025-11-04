#include "TextInterface.h"

#include "QFluent/LineEdit.h"

TextInterface::TextInterface(QWidget *parent)
    : GalleryInterface("文本", "qfluentwidgets.components.widgets", parent)
{
    setObjectName("TextInterface");

    auto lineEdit = new LineEdit(this);
    lineEdit->setClearButtonEnabled(true);
    lineEdit->setText("ko no dio da!");
    addExampleCard("带清空按钮的输入框", lineEdit);
}


