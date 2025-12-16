#include "DialogInputInterface.h"

#include "Theme.h"
#include "QFluent/Label.h"
#include "QFluent/Flyout.h"
#include "QFluent/Loading.h"
#include "QFluent/LineEdit.h"
#include "QFluent/PushButton.h"
#include "QFluent/TeachingTip.h"
#include "QFluent/Dialog/ColorDialog.h"
#include "QFluent/Dialog/MessageDialog.h"
#include "QFluent/Dialog/MessageBoxBase.h"

DialogInputInterface::DialogInputInterface(QWidget *parent)
    : GalleryInterface("对话框", "", parent)
{
    setObjectName("DialogInputInterface");

    auto messageDialogBtn = new PushButton("显示对话框", this);
    connect(messageDialogBtn, &PushButton::clicked, this, [this](){
        auto box = new MessageDialog("这是一个带有遮罩的对话框",
                                     "这是一个带有遮罩的对话框这是一个带有遮罩的对话框这是一个带有遮罩的对话框这是一个带有遮罩的对话框"
                                     "这是一个带有遮罩的对话框这是一个带有遮罩的对话框这是一个带有遮罩的对话框这是一个带有遮罩的对话框",
                                     this->window());
        box->setIsClosableOnMaskClicked(true);
        box->exec();
    });
    addExampleCard("带遮罩的对话框", messageDialogBtn);

    auto loadingDialogBtn = new PushButton("显示对话框", this);
    connect(loadingDialogBtn, &PushButton::clicked, this, [this](){
        static Loading *load = new Loading(QString(), this->window());
        load->exec();
    });
    addExampleCard("加载中等待窗口", loadingDialogBtn);

    auto customDialogBtn = new PushButton("显示对话框", this);
    auto customDialog = new MessageBoxBase(this->window());
    customDialog->viewLayout()->addWidget(new SubtitleLabel("打开 URL", customDialog));
    customDialog->viewLayout()->addWidget(new LineEdit(customDialog));
    customDialog->centerWidget()->setFixedWidth(360);
    customDialog->close();

    connect(customDialogBtn, &PushButton::clicked, this, [customDialog](){
        customDialog->exec();
    });
    addExampleCard("自定义对话框", customDialogBtn);

    auto colorDialogBtn = new PushButton("显示对话框", this);
    connect(colorDialogBtn, &PushButton::clicked, this, [=](){
        static auto colorDialog = new ColorDialog(Theme::instance()->themeColor(), "选择颜色", this->window());
        colorDialog->exec();
    });
    addExampleCard("颜色对话框", colorDialogBtn);

    auto simpleFlyoutButton = new PushButton("显示浮出控件", this);
    connect(simpleFlyoutButton, &PushButton::clicked, this, [=](){ showSimpleFlyout(simpleFlyoutButton); });
    addExampleCard("简单浮出控件", simpleFlyoutButton);

    auto complexFlyoutButton = new PushButton("显示浮出控件", this);
    connect(complexFlyoutButton, &PushButton::clicked, this, [=](){ showComplexFlyout(complexFlyoutButton); });
    addExampleCard("带图片和按钮的浮出控件", complexFlyoutButton);

    auto teachingButton = new PushButton("显示气泡弹窗", this);
    connect(teachingButton, &PushButton::clicked, this, [=](){ showBottomTeachingTip(teachingButton); });
    addExampleCard("气泡弹窗", teachingButton);

    auto teachingRightButton = new PushButton("显示气泡弹窗", this);
    connect(teachingRightButton, &PushButton::clicked, this, [=](){ showLeftBottomTeachingTip(teachingRightButton); });
    addExampleCard("带图片和按钮的气泡弹窗", teachingRightButton);

}

void DialogInputInterface::showSimpleFlyout(QWidget* target)
{
    Flyout::create("你看", "我有几分有几分有几分像从前?", FluentIcon(Fluent::IconType::LEAF).qicon(), QPixmap(), false, target, this);
}

void DialogInputInterface::showComplexFlyout(QWidget* target)
{
    auto view = new FlyoutView("iPhone", "iPhone 17 Pro 及 iPhone 17 Pro Max 新登場，由內而外\n精心設計，創出空前最強 iPhone 型號。熱鍛鋁金屬一體式\n機身為嶄新設計核心，將效能表現、電池容量以及耐用度極級提升。",
                               QIcon(), QPixmap(":/res/SBR.jpg"));

    auto button = new PushButton("Action");
    button->setFixedWidth(120);
    view->addWidget(button, Qt::AlignCenter);

    view->widgetLayout()->insertSpacing(1, 5);
    view->widgetLayout()->insertSpacing(0, 5);
    view->widgetLayout()->addSpacing(5);

    Flyout::make(view, target, this->window(), FlyoutAnimationType::SLIDE_RIGHT);
}

void DialogInputInterface::showBottomTeachingTip(QWidget* target)
{
    TeachingTip::create(target, "iPhone", "iPhone 17 Pro 及 iPhone 17 Pro Max 新登場，由內而外精心設計，創出空前最強 iPhone 型號。熱鍛鋁金屬一體式機身為嶄新設計核心，將效能表現、電池容量以及耐用度極級提升。",
                        FluentIcon(Fluent::IconType::LEAF).qicon(), QPixmap(), true, -1, TeachingTipTailPosition::BOTTOM, this);
}

void DialogInputInterface::showLeftBottomTeachingTip(QWidget* target)
{
    auto view = new TeachingTipView("iPhone", "iPhone 17 Pro 及 iPhone 17 Pro Max 新登場，由內而外精心設計，創出空前最強 iPhone 型號。熱鍛鋁金屬一體式機身為嶄新設計核心，將效能表現、電池容量以及耐用度極級提升。",
                                    QIcon(), QPixmap(":/res/SBR.jpg"), true, TeachingTipTailPosition::LEFT_BOTTOM);

    auto button = new PushButton("Action");
    button->setFixedWidth(120);
    view->addWidget(button, Qt::AlignCenter);

    auto t = TeachingTip::make(view, target, 3000, TeachingTipTailPosition::LEFT_BOTTOM, this);
    connect(view, &TeachingTipView::closed, this, [t](){ t->close(); });
}



