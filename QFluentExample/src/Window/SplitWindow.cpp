#include "SplitWindow.h"

#include <QHBoxLayout>

#include "StyleSheet.h"
#include "QFluent/Label.h"
#include "QFluent/StackedWidget.h"
#include "QFluent/navigation/NavigationBar.h"

SplitWidget::SplitWidget()
{
    setWindowButtonFlags(AppBarType::IconButtonHint | AppBarType::WindowTitleHint |
                         AppBarType::MinimizeButtonHint | AppBarType::MaximizeButtonHint |
                         AppBarType::CloseButtonHint | AppBarType::RouteBackButtonHint);

    setWindowDisplayMode(ApplicationType::Normal);

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

    addSubInterface("1", FluentIcon(FluentIconType::HOME), "主页", createWidget("主页", this), true, NavigationType::TOP);
    navigationInterface()->addSeparator();
    addSubInterface("3", FluentIcon(FluentIconType::CHECKBOX), "输入", createWidget("输入", this), true, NavigationType::SCROLL);
    addSubInterface("4", FluentIcon(FluentIconType::DATE_TIME), "日期", createWidget("日期", this), true, NavigationType::SCROLL);
    navigationInterface()->addSeparator(NavigationType::NavigationItemPosition::BOTTOM);
    addSubInterface("5", FluentIcon(FluentIconType::SETTING), "设置", createWidget("设置", this), true, NavigationType::BOTTOM);

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
