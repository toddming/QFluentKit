#include "LayoutInterface.h"

#include <QWidget>

#include "QFluent/PushButton.h"
#include "QFluent/Layout/FlowLayout.h"

LayoutInterface::LayoutInterface(QWidget *parent)
    : GalleryInterface("布局", "", parent)
{
    setObjectName("LayoutInterface");

    addExampleCard("不带动画效果的流式布局", createWidget(false), "", 1);
    addExampleCard("带动画效果的流式布局", createWidget(true), "", 1);
}


QWidget *LayoutInterface::createWidget(bool animation)
{
    QStringList texts;
    texts << "随便点点"
          << "点到为止"
          << "手滑勿怪"
          << "假装很忙"
          << "正在摸鱼"
          << "别信这个"
          << "纯属虚构"
          << "演示专用"
          << "点完就跑"
          << "无效操作"
          << "仅供观赏";
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
