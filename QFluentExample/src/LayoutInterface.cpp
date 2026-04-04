#include "LayoutInterface.h"

#include <QWidget>
#include "QFluent/Label.h"
#include "QFluent/Splitter.h"
#include "QFluent/PushButton.h"
#include "QFluent/Layout/FlowLayout.h"
#include "QFluent/Layout/AdaptiveFlowLayout.h"

LayoutInterface::LayoutInterface(QWidget *parent)
    : GalleryInterface("布局", "", parent)
{
    setObjectName("LayoutInterface");

    addExampleCard("不带动画效果的流式布局", createWidget(false), "", 1);
    addExampleCard("带动画效果的流式布局", createWidget(true), "", 1);

    addExampleCard("自适应流式布局", createAdaptiveWidget(), "", 1);

    addExampleCard("拆分式布局", createSplitterWidget(), "", 1);
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

QWidget *LayoutInterface::createAdaptiveWidget()
{
    QStringList texts;
    texts << "自适应布局"
          << "自动换行"
          << "宽度自适应"
          << "均匀分布"
          << "流式排列";

    auto widget = new QWidget();
    auto layout = new AdaptiveFlowLayout(widget, false);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setVerticalSpacing(20);
    layout->setHorizontalSpacing(10);
    layout->setWidgetMinimumWidth(150);

    foreach (const QString &text, texts) {
        layout->addWidget(new PushButton(text, widget));
    }

    // 在索引 1 处插入一个主按钮
    auto primaryBtn = new PrimaryPushButton("插入按钮", widget);
    layout->insertWidget(1, primaryBtn);

    return widget;
}

QWidget *LayoutInterface::createSplitterWidget()
{
    auto widget = new QWidget();
    widget->setFixedHeight(200);
    auto layout = new QHBoxLayout(widget);
    auto splitter = new Splitter(Qt::Horizontal, widget);
    layout->addWidget(splitter);

    for (int i=0; i < 4; i++) {
        auto label = new BodyLabel(QString("页面%1").arg(i+1), widget);
        label->setAlignment(Qt::AlignCenter);
        splitter->addWidget(label);
    }
    splitter->setLineWidth(8);
    return widget;
}
