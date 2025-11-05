#include "StatusInfoInterface.h"

#include "QFluent/InfoBar.h"
#include "QFluent/PushButton.h"

StatusInfoInterface::StatusInfoInterface(QWidget *parent)
    : GalleryInterface("状态", "qfluentwidgets.components.widgets", parent)
{
    setObjectName("StatusInfoInterface");

    auto w = new QWidget(this);
    auto hBoxLayout = new QHBoxLayout(w);
    auto btn1 = new PushButton("右上角", w);
    auto btn2 = new PushButton("顶部居中", w);
    auto btn3 = new PushButton("左上角", w);
    auto btn4 = new PushButton("右下角", w);
    auto btn5 = new PushButton("底部居中", w);
    auto btn6 = new PushButton("左下角", w);
    connect(btn1, &PushButton::clicked, this, [=](){
        InfoBar::info("你看", "我有几分像从前?", Qt::Horizontal, false, 2000, InfoBarType::BarPosition::TOP_RIGHT, this);
    });
    connect(btn2, &PushButton::clicked, this, [=](){
        InfoBar::success("你看", "我有几分像从前?", Qt::Horizontal, true, 2000, InfoBarType::BarPosition::TOP, this);
    });
    connect(btn3, &PushButton::clicked, this, [=](){
        InfoBar::warning("你看", "我有几分像从前?", Qt::Horizontal, false, 2000, InfoBarType::BarPosition::TOP_LEFT, this);
    });
    connect(btn4, &PushButton::clicked, this, [=](){
        InfoBar::error("你看", "我有几分像从前?", Qt::Horizontal, false, 2000, InfoBarType::BarPosition::BOTTOM_RIGHT, this);
    });
    connect(btn5, &PushButton::clicked, this, [=](){
        InfoBar::success("你看", "我有几分像从前?", Qt::Horizontal, false, 2000, InfoBarType::BarPosition::BOTTOM, this);
    });
    connect(btn6, &PushButton::clicked, this, [=](){
        InfoBar::warning("你看", "我有几分像从前?", Qt::Horizontal, false, 2000, InfoBarType::BarPosition::BOTTOM_LEFT, this);
    });
    hBoxLayout->addWidget(btn1);
    hBoxLayout->addWidget(btn2);
    hBoxLayout->addWidget(btn3);
    hBoxLayout->addWidget(btn4);
    hBoxLayout->addWidget(btn5);
    hBoxLayout->addWidget(btn6);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    hBoxLayout->setSpacing(15);

    addExampleCard("不同弹出位置的消息条", w);
}


