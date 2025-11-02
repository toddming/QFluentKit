#include "LayoutInterface.h"

#include <QWidget>

#include "QFluent/PushButton.h"
#include "QFluent/layout/FlowLayout.h"

LayoutInterface::LayoutInterface(QWidget *parent)
    : GalleryInterface("布局", "qfluentwidgets.components.widgets", parent)
{
    setObjectName("LayoutInterface");

    addExampleCard("不带动画效果的流式布局", createWidget(false), "", 1);
    addExampleCard("带动画效果的流式布局", createWidget(true), "", 1);
}


QWidget *LayoutInterface::createWidget(bool animation)
{
    QStringList texts;
    texts << "白金之星"
          << "法皇之绿"
          << "银色战车"
          << "疯狂钻石"
          << "黄金体验"
          << "天堂之门"
          << "杀手皇后"
          << "石之自由"
          << "绯红之王"
          << "性感手枪";
    auto widget = new QWidget();
    auto layout = new FlowLayout(widget, animation);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setVerticalSpacing(20);
    layout->setHorizontalSpacing(10);

    foreach (const QString &text, texts) {
        layout->addWidget(new PushButton(text, widget));
    }
    return widget;
}
