#include "SplitWindow.h"

#include <QHBoxLayout>

#include "StyleSheet.h"
#include "QFluent/Label.h"
#include "QFluent/StackedWidget.h"
#include "QFluent/navigation/NavigationBar.h"

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

    navigationInterface()->avatarWidget()->hide();
    navigationInterface()->setExpandWidth(200);

    addSubInterface("1", FluentIcon(Fluent::IconType::HOME), "主页", createWidget("主页", this), true, Fluent::NavigationItemPosition::TOP);
    navigationInterface()->addSeparator();
    addSubInterface("3", FluentIcon(Fluent::IconType::CHECKBOX), "输入", createWidget("输入", this), true, Fluent::NavigationItemPosition::SCROLL);
    addSubInterface("4", FluentIcon(Fluent::IconType::DATE_TIME), "日期", createWidget("日期", this), true, Fluent::NavigationItemPosition::SCROLL);
    navigationInterface()->addSeparator(Fluent::NavigationItemPosition::BOTTOM);
    addSubInterface("5", FluentIcon(Fluent::IconType::SETTING), "设置", createWidget("设置", this), true, Fluent::NavigationItemPosition::BOTTOM);

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
