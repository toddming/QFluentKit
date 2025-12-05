#include "MainWindow.h"

#include "Router.h"
#include "QFluent/navigation/NavigationPanel.h"
#include "QFluent/navigation/NavigationWidget.h"

#include "FluentIcon.h"
#include "HomeInterface.h"
#include "IconInterface.h"
#include "BasicInputInterface.h"
#include "SettingInterface.h"
#include "DateTimeInputInterface.h"
#include "DialogInputInterface.h"
#include "LayoutInterface.h"
#include "MenuInterface.h"
#include "NavigationViewInterface.h"
#include "ScrollInterface.h"
#include "StatusInfoInterface.h"
#include "TextInterface.h"
#include "ViewInterface.h"
#include "WindowInterface.h"

#include "ConfigManager.h"

MainWindow::MainWindow()
{
    setWindowTitle("QFluentKit");
    setWindowIcon(QPixmap(":/res/example.png"));
    resize(1024, 768);

    setWindowButtonHints(windowButtonHints() | Fluent::WindowButtonHint::RouteBack);

    int theme = ConfigManager::instance().getValue("Window/theme", 0).toInt();
    Theme::instance()->setThemeColor(QColor(ConfigManager::instance().getValue("Window/color", "#0066b4").toString()));
    Theme::instance()->setTheme(theme == 0 ? Fluent::ThemeMode::DARK : Fluent::ThemeMode::LIGHT);

    navigationInterface()->setExpandWidth(200);

    QString iconPath = QString(":/res/icons/%1_{color}.svg");
    addSubInterface("1", FluentIcon(Fluent::IconType::HOME), "主页", new HomeInterface(this), true, Fluent::NavigationItemPosition::TOP);
    addSubInterface("2", FluentIcon(Fluent::IconType::EMOJI_TAB_SYMBOLS), "图标", new IconInterface(this), true, Fluent::NavigationItemPosition::TOP);
    navigationInterface()->addSeparator();
    addSubInterface("3", FluentIcon(Fluent::IconType::CHECKBOX), "基本输入", new BasicInputInterface(this), true, Fluent::NavigationItemPosition::SCROLL);
    addSubInterface("4", FluentIcon(Fluent::IconType::DATE_TIME), "日期和时间", new DateTimeInputInterface(this), true, Fluent::NavigationItemPosition::SCROLL);
    addSubInterface("5", FluentIcon(Fluent::IconType::MESSAGE), "对话框", new DialogInputInterface(this), true, Fluent::NavigationItemPosition::SCROLL);
    addSubInterface("6", FluentIcon(Fluent::IconType::LAYOUT), "布局", new LayoutInterface(this), true, Fluent::NavigationItemPosition::SCROLL);
    addSubInterface("7", FluentIcon(iconPath.arg("Menu")), "菜单", new MenuInterface(this), true, Fluent::NavigationItemPosition::SCROLL);
    addSubInterface("8", FluentIcon(Fluent::IconType::MENU), "导航", new NavigationViewInterface(this), true, Fluent::NavigationItemPosition::SCROLL);
    addSubInterface("9", FluentIcon(Fluent::IconType::APPLICATION), "窗口", new WindowInterface(this), true, Fluent::NavigationItemPosition::SCROLL);
    addSubInterface("10", FluentIcon(Fluent::IconType::SCROLL), "滚动", new ScrollInterface(this), true, Fluent::NavigationItemPosition::SCROLL);
    addSubInterface("11", FluentIcon(Fluent::IconType::CHAT), "状态", new StatusInfoInterface(this), true, Fluent::NavigationItemPosition::SCROLL);
    addSubInterface("12", FluentIcon(iconPath.arg("Text")), "文本", new TextInterface(this), true, Fluent::NavigationItemPosition::SCROLL);
    addSubInterface("13", FluentIcon(iconPath.arg("Grid")), "视图", new ViewInterface(this), true, Fluent::NavigationItemPosition::SCROLL);

    auto sub1 = new QWidget(this);
    sub1->setObjectName("sub1");

    auto sub11 = new QWidget(this);
    sub11->setObjectName("sub11");
    auto sub12 = new QWidget(this);
    sub12->setObjectName("sub12");

    auto sub2 = new QWidget(this);
    sub2->setObjectName("sub2");
    auto sub21 = new QWidget(this);
    sub21->setObjectName("sub21");
    auto sub22 = new QWidget(this);
    sub22->setObjectName("sub22");
    auto sub221 = new QWidget(this);
    sub221->setObjectName("sub221");
    auto sub222 = new QWidget(this);
    sub222->setObjectName("sub222");

    auto sub3 = new QWidget(this);
    sub3->setObjectName("sub3");
    auto sub31 = new QWidget(this);
    sub31->setObjectName("sub31");
    auto sub32 = new QWidget(this);
    sub32->setObjectName("sub32");

    addSubInterface("15", FluentIcon(iconPath.arg("Grid")), "SUB1", sub1, true, Fluent::NavigationItemPosition::SCROLL, "", "13");
    addSubInterface("18", FluentIcon(iconPath.arg("Grid")), "SUB11", sub11, true, Fluent::NavigationItemPosition::SCROLL, "", "15");
    addSubInterface("19", FluentIcon(iconPath.arg("Grid")), "SUB12", sub12, true, Fluent::NavigationItemPosition::SCROLL, "", "15");

    addSubInterface("16", FluentIcon(iconPath.arg("Grid")), "SUB2", sub2, true, Fluent::NavigationItemPosition::SCROLL, "", "13");
    addSubInterface("20", FluentIcon(iconPath.arg("Grid")), "SUB21", sub21, true, Fluent::NavigationItemPosition::SCROLL, "", "16");
    addSubInterface("21", FluentIcon(iconPath.arg("Grid")), "SUB22", sub22, true, Fluent::NavigationItemPosition::SCROLL, "", "16");
    addSubInterface("24", FluentIcon(iconPath.arg("Grid")), "SUB221", sub221, true, Fluent::NavigationItemPosition::SCROLL, "", "21");
    addSubInterface("25", FluentIcon(iconPath.arg("Grid")), "SUB222", sub222, true, Fluent::NavigationItemPosition::SCROLL, "", "21");

    addSubInterface("17", FluentIcon(iconPath.arg("Grid")), "SUB3", sub3, true, Fluent::NavigationItemPosition::SCROLL, "", "13");
    addSubInterface("22", FluentIcon(iconPath.arg("Grid")), "SUB31", sub31, true, Fluent::NavigationItemPosition::SCROLL, "", "17");
    addSubInterface("23", FluentIcon(iconPath.arg("Grid")), "SUB32", sub32, true, Fluent::NavigationItemPosition::SCROLL, "", "17");

    navigationInterface()->addSeparator(Fluent::NavigationItemPosition::BOTTOM);
    addSubInterface("14", FluentIcon(Fluent::IconType::SETTING), "设置", new SettingInterface(this), true, Fluent::NavigationItemPosition::BOTTOM);

    qrouter->setDefaultRouteKey(stackedWidget(), "homeInterface");
    navigationInterface()->setCurrentItem("1");

    NavigationAvatarWidget* avatar = navigationInterface()->avatarWidget();
    avatar->setName("Administrator");
    avatar->setAvatar(QImage(":/res/avatar.png"));

    connect(this, &MainWindow::backRequested, this, [=](){
        qrouter->pop();
    });
}

void MainWindow::setCurrentInterface(const QString &routeKey, int index)
{
    qrouter->push(stackedWidget(), routeKey);
    navigationInterface()->setCurrentItem(QString::number(index));
}
