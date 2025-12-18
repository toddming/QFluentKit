#include "MainWindow.h"

#include "Router.h"
#include "QFluent/Navigation/NavigationPanel.h"
#include "QFluent/Navigation/NavigationWidget.h"
#include "QFluent/Dialog/MessageDialog.h"

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

using NavPos = Fluent::NavigationItemPosition;

MainWindow::MainWindow()
{
    setWindowTitle("QFluentKit");
    setWindowIcon(QPixmap(":/res/example.png"));
    resize(1024, 768);

    setWindowButtonHints(windowButtonHints() | Fluent::WindowButtonHint::RouteBack);

    int theme = ConfigManager::instance().getValue("Window/theme", 0).toInt();
    Theme::instance()->setThemeColor(QColor(ConfigManager::instance().getValue("Window/color", "#0066b4").toString()));
    Theme::instance()->setTheme(theme == 0 ? Fluent::ThemeMode::DARK : Fluent::ThemeMode::LIGHT);

    navigationInterface()->setExpandWidth(240);
    auto userCard = navigationInterface()->addUserCard("userCard", ":/res/Shizuka.png", "Shizuka", "shizuka@gmail.com",
                                       nullptr, NavPos::TOP, false);
    userCard->setTitleFontSize(12);
    userCard->setSubtitleFontSize(10);

    QString iconPath = QString(":/res/icons/%1_{color}.svg");
    addSubInterface("1", FluentIcon(Fluent::IconType::HOME), "主页", new HomeInterface(this), true, NavPos::TOP);
    addSubInterface("2", FluentIcon(Fluent::IconType::EMOJI_TAB_SYMBOLS), "图标", new IconInterface(this), true, NavPos::TOP);
    navigationInterface()->addSeparator();
    addSubInterface("3", FluentIcon(Fluent::IconType::CHECKBOX), "基本输入", new BasicInputInterface(this), true, NavPos::SCROLL);
    addSubInterface("4", FluentIcon(Fluent::IconType::DATE_TIME), "日期和时间", new DateTimeInputInterface(this), true, NavPos::SCROLL);
    addSubInterface("5", FluentIcon(Fluent::IconType::MESSAGE), "对话框", new DialogInputInterface(this), true, NavPos::SCROLL);
    addSubInterface("6", FluentIcon(Fluent::IconType::LAYOUT), "布局", new LayoutInterface(this), true, NavPos::SCROLL);
    addSubInterface("7", FluentIcon(iconPath.arg("Menu")), "菜单", new MenuInterface(this), true, NavPos::SCROLL);
    addSubInterface("8", FluentIcon(Fluent::IconType::MENU), "导航", new NavigationViewInterface(this), true, NavPos::SCROLL);
    addSubInterface("9", FluentIcon(Fluent::IconType::APPLICATION), "窗口", new WindowInterface(this), true, NavPos::SCROLL);
    addSubInterface("10", FluentIcon(Fluent::IconType::SCROLL), "滚动", new ScrollInterface(this), true, NavPos::SCROLL);
    addSubInterface("11", FluentIcon(Fluent::IconType::CHAT), "状态", new StatusInfoInterface(this), true, NavPos::SCROLL);
    addSubInterface("12", FluentIcon(iconPath.arg("Text")), "文本", new TextInterface(this), true, NavPos::SCROLL);
    addSubInterface("13", FluentIcon(iconPath.arg("Grid")), "视图", new ViewInterface(this), true, NavPos::SCROLL);

    navigationInterface()->addSeparator(NavPos::BOTTOM);
    addSubInterface("15", FluentIcon(Fluent::IconType::SETTING), "设置", new SettingInterface(this), true, NavPos::BOTTOM);

    qrouter->setDefaultRouteKey(stackedWidget(), "homeInterface");
    navigationInterface()->setCurrentItem("1");

    connect(this, &MainWindow::backRequested, this, [=](){
        qrouter->pop();
    });
}

void MainWindow::setCurrentInterface(const QString &routeKey, int index)
{
    qrouter->push(stackedWidget(), routeKey);
    navigationInterface()->setCurrentItem(QString::number(index));
}

void MainWindow::showDialog()
{
    auto box = new MessageDialog("你是遇到问题了吗?",
                                 "遇到问题？欢迎加我 QQ（1912229135）反馈～看到后我会第一时间修复，感谢你让这个项目变得越来越棒！",
                                 this->window());
    box->setIsClosableOnMaskClicked(true);
    box->exec();
}
