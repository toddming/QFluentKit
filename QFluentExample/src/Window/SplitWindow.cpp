#include "SplitWindow.h"

#include <QHBoxLayout>

#include "StyleSheet.h"
#include "QFluent/Label.h"
#include "QFluent/StackedWidget.h"
#include "QFluent/Navigation/NavigationBar.h"

using NavPos = Fluent::NavigationItemPosition;

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

    addSubInterface("1", FluentIcon(Fluent::IconType::HOME), "主页", createWidget("主页", this), true, NavPos::TOP);
    navigationInterface()->addSeparator();
    navigationInterface()->addItemHeader("基础", NavPos::SCROLL);
    addSubInterface("3", FluentIcon(Fluent::IconType::CHAT), "对话", createWidget("对话", this), true, NavPos::SCROLL);
    addSubInterface("4", FluentIcon(Fluent::IconType::GAME), "游戏", createWidget("游戏", this), true, NavPos::SCROLL);

    navigationInterface()->addItemHeader("进阶", NavPos::SCROLL);
    addSubInterface("5", FluentIcon(Fluent::IconType::LEAF), "能效", createWidget("能效", this), true, NavPos::SCROLL);
    addSubInterface("6", FluentIcon(Fluent::IconType::FINGERPRINT), "加密", createWidget("加密", this), true, NavPos::SCROLL);
    navigationInterface()->addSeparator(NavPos::BOTTOM);
    auto avatarWidget = new NavigationAvatarWidget("Administrator", QImage(":/res/avatar.png"), this);
    navigationInterface()->addWidget("7", avatarWidget, nullptr, NavPos::BOTTOM);

    addSubInterface("8", FluentIcon(Fluent::IconType::SETTING), "设置", createWidget("设置", this), true, NavPos::BOTTOM);

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
