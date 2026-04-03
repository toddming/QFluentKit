#include "NavbarWindow.h"

#include <QHBoxLayout>

#include "StyleSheet.h"
#include "QFluent/Label.h"
#include "QFluent/StackedWidget.h"
#include "QFluent/Navigation/NavigationBar.h"

NavbarWidget::NavbarWidget()
{
    setWindowButtonHints(WindowButtonHint::WindowIcon | WindowButtonHint::Title |
                         WindowButtonHint::Minimize | WindowButtonHint::Maximize |
                         WindowButtonHint::Close);

    setContentsMargins(0, 0, 0, 0);

    resize(800, 600);

    initUI();
}

void NavbarWidget::initUI()
{
    setWindowTitle("QFluentKit");
    setWindowIcon(QPixmap(":/res/example.png"));

    auto hBoxLayout = new QHBoxLayout(this);
    hBoxLayout->setSpacing(0);

#if USE_QWINDOWKIT
    hBoxLayout->setContentsMargins(0, 45, 0, 0);
#else
    hBoxLayout->setContentsMargins(0, 5, 0, 0);
#endif

    auto stacked = new StackedWidget(this);
    stacked->addWidget(createWidget("主页", stacked));
    stacked->addWidget(createWidget("输入", stacked));
    stacked->addWidget(createWidget("日期", stacked));
    stacked->addWidget(createWidget("信息框", stacked));
    stacked->addWidget(createWidget("设置", stacked));

    auto *navigationBar = new NavigationBar(this);
    navigationBar->addItem("1", FluentIcon(Fluent::IconType::HOME), "主页", [stacked](){stacked->setCurrentIndex(0, false);}, true, NavigationPanel::ItemPosition::TOP);
    navigationBar->addSeparator();
    navigationBar->addItem("2", FluentIcon(Fluent::IconType::CHECKBOX), "输入", [stacked](){stacked->setCurrentIndex(1, false);}, true, NavigationPanel::ItemPosition::SCROLL);
    navigationBar->addItem("3", FluentIcon(Fluent::IconType::DATE_TIME), "日期", [stacked](){stacked->setCurrentIndex(2, false);}, true, NavigationPanel::ItemPosition::SCROLL);
    navigationBar->addItem("4", FluentIcon(Fluent::IconType::MESSAGE), "信息框", [stacked](){stacked->setCurrentIndex(3, false);}, true, NavigationPanel::ItemPosition::SCROLL);
    navigationBar->addSeparator(NavigationPanel::ItemPosition::BOTTOM);
    navigationBar->addItem("5", FluentIcon(Fluent::IconType::SETTING), "设置", [stacked](){stacked->setCurrentIndex(4, false);}, true, NavigationPanel::ItemPosition::BOTTOM);
    navigationBar->setCurrentItem("1");

    hBoxLayout->addWidget(navigationBar, 0);
    hBoxLayout->addWidget(stacked, 1);
}

QWidget* NavbarWidget::createWidget(const QString &text, QWidget* parent)
{
    auto w = new QWidget(parent);
    auto hLay = new QHBoxLayout(w);
    auto label = new DisplayLabel(text, w);
    label->setAlignment(Qt::AlignCenter);
    hLay->addWidget(label);
    return w;
}
