#include "SplitWindow.h"

#include <QHBoxLayout>

#include "StyleSheet.h"
#include "QFluent/Label.h"
#include "QFluent/StackedWidget.h"
#include "QFluent/Navigation/NavigationBar.h"

using FIT = Fluent::IconType;
using NIP = Fluent::NavigationItemPosition;

SplitWidget::SplitWidget()
{

    setWindowButtonHints(Fluent::WindowButtonHint::Icon | Fluent::WindowButtonHint::Title |
                         Fluent::WindowButtonHint::Minimize | Fluent::WindowButtonHint::Maximize |
                         Fluent::WindowButtonHint::Close | Fluent::WindowButtonHint::RouteBack);

    setWindowEffect(Fluent::WindowEffect::Normal);

    setContentsMargins(0, 0, 0, 0);

    resize(800, 600);

    initUI();
}

void SplitWidget::initUI()
{
    setWindowTitle("QFluentKit");
    setWindowIcon(QPixmap(":/res/example.png"));

    navigationInterface()->setExpandWidth(200);

    addSubInterface("1", FluentIcon(FIT::HOME), "主页", createWidget("主页", this), true, NIP::TOP);
    navigationInterface()->addSeparator();
    navigationInterface()->addItemHeader("基础", NIP::SCROLL);
    addSubInterface("3", FluentIcon(FIT::CHAT), "对话", createWidget("对话", this), true, NIP::SCROLL);
    addSubInterface("4", FluentIcon(FIT::GAME), "游戏", createWidget("游戏", this), true, NIP::SCROLL);

    navigationInterface()->addItemHeader("进阶", NIP::SCROLL);
    addSubInterface("5", FluentIcon(FIT::LEAF), "能效", createWidget("能效", this), true, NIP::SCROLL);
    addSubInterface("6", FluentIcon(FIT::FINGERPRINT), "加密", createWidget("加密", this), true, NIP::SCROLL);
    navigationInterface()->addSeparator(NIP::BOTTOM);
    auto avatarWidget = new NavigationAvatarWidget("Administrator", QImage(":/res/avatar.png"), this);
    navigationInterface()->addWidget("7", avatarWidget, nullptr, NIP::BOTTOM);

    addSubInterface("8", FluentIcon(FIT::SETTING), "设置", createWidget("设置", this), true, NIP::BOTTOM);

    navigationInterface()->setCurrentItem("1");

}

QWidget* SplitWidget::createWidget(const QString &text, QWidget* parent)
{
    auto w = new QWidget(parent);
    auto hLay = new QHBoxLayout(w);
    auto label = new DisplayLabel(text, w);
    label->setAlignment(Qt::AlignCenter);
    hLay->addWidget(label);
    return w;
}
