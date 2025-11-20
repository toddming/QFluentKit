#include "StatusInfoInterface.h"

#include "Theme.h"
#include "QFluent/Label.h"
#include "QFluent/SpinBox.h"
#include "QFluent/InfoBar.h"
#include "QFluent/PushButton.h"
#include "QFluent/progress/ProgressBar.h"
#include "QFluent/progress/ProgressRing.h"
#include "QFluent/progress/IndeterminateProgressBar.h"
#include "QFluent/progress/IndeterminateProgressRing.h"

StatusInfoInterface::StatusInfoInterface(QWidget *parent)
    : GalleryInterface("状态", "", parent)
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
        InfoBar::info("你看", "我有几分像从前?", Qt::Horizontal, false, 2000, Fluent::MessagePosition::TOP_RIGHT, this);
    });
    connect(btn2, &PushButton::clicked, this, [=](){
        InfoBar::success("你看", "我有几分有几分有几分有几分有几分有几分有几分有几分有几分有几分有几分有几分有几分有几分"
                               "有几分有几分有几分有几分有几分有几分有几分有几分有几分有几分有几分有几分有几分有几分像从前?",
                         Qt::Horizontal, true, 2000, Fluent::MessagePosition::TOP, this);
    });
    connect(btn3, &PushButton::clicked, this, [=](){
        InfoBar::warning("你看", "我有几分像从前?", Qt::Horizontal, false, 2000, Fluent::MessagePosition::TOP_LEFT, this);
    });
    connect(btn4, &PushButton::clicked, this, [=](){
        InfoBar::error("你看", "我有几分像从前?", Qt::Horizontal, false, 2000, Fluent::MessagePosition::BOTTOM_RIGHT, this);
    });
    connect(btn5, &PushButton::clicked, this, [=](){
        InfoBar::success("你看", "我有几分像从前?", Qt::Horizontal, false, 2000, Fluent::MessagePosition::BOTTOM, this);
    });
    connect(btn6, &PushButton::clicked, this, [=](){
        InfoBar::warning("你看", "我有几分像从前?", Qt::Horizontal, false, 2000, Fluent::MessagePosition::BOTTOM_LEFT, this);
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

    QColor color = Theme::instance()->themeColor();
    auto indeterminateProgressBar = new IndeterminateProgressBar(this);
    indeterminateProgressBar->setFixedWidth(200);
    indeterminateProgressBar->setCustomBarColor(color, color);
    addExampleCard("不确定进度条", indeterminateProgressBar);

    auto progressBar = new ProgressBar(this);
    progressBar->setFixedWidth(200);
    progressBar->setValue(50);
    progressBar->setCustomBarColor(color, color);
    addExampleCard("确定的进度条", createProgressWidget(progressBar));

    auto indeterminateProgressRing = new IndeterminateProgressRing(this);
    indeterminateProgressRing->setFixedSize(70, 70);
    indeterminateProgressRing->setCustomBarColor(color, color);
    addExampleCard("不确定进度环", indeterminateProgressRing);

    auto progressRing = new ProgressRing(this);
    progressRing->setValue(50);
    progressRing->setFixedSize(70, 70);
    progressRing->setCustomBarColor(color, color);
    addExampleCard("确定的进度环", createProgressWidget(progressRing));
}


QWidget* StatusInfoInterface::createProgressWidget(QProgressBar *widget)
{
    auto w = new QWidget(this);
    auto hBoxLayout = new QHBoxLayout(w);
    auto spinBox = new SpinBox(w);
    spinBox->setValue(50);
    spinBox->setRange(0, 100);

    hBoxLayout->addWidget(widget);
    hBoxLayout->addSpacing(50);
    hBoxLayout->addWidget(new BodyLabel("进度", w));
    hBoxLayout->addSpacing(5);
    hBoxLayout->addWidget(spinBox);
    setContentsMargins(0, 0, 0, 0);

    connect(spinBox, QOverload<int>::of(&SpinBox::valueChanged), this, [this, widget](int value){
        widget->setValue(value);
    });

    return w;
}

