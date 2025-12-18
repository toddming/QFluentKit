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

using FIT = Fluent::IconType;
using NIP = Fluent::NavigationItemPosition;

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
                                       nullptr, NIP::TOP, false);
    userCard->setTitleFontSize(12);
    userCard->setSubtitleFontSize(10);

    QString iconPath = QString(":/res/icons/%1_{color}.svg");
    addSubInterface("1", FluentIcon(FIT::HOME), "主页", new HomeInterface(this), true, NIP::TOP);
    addSubInterface("2", FluentIcon(FIT::EMOJI_TAB_SYMBOLS), "图标", new IconInterface(this), true, NIP::TOP);
    navigationInterface()->addSeparator();
    addSubInterface("3", FluentIcon(FIT::CHECKBOX), "基本输入", new BasicInputInterface(this), true, NIP::SCROLL);
    addSubInterface("4", FluentIcon(FIT::DATE_TIME), "日期和时间", new DateTimeInputInterface(this), true, NIP::SCROLL);
    addSubInterface("5", FluentIcon(FIT::MESSAGE), "对话框", new DialogInputInterface(this), true, NIP::SCROLL);
    addSubInterface("6", FluentIcon(FIT::LAYOUT), "布局", new LayoutInterface(this), true, NIP::SCROLL);
    addSubInterface("7", FluentIcon(iconPath.arg("Menu")), "菜单", new MenuInterface(this), true, NIP::SCROLL);
    addSubInterface("8", FluentIcon(FIT::MENU), "导航", new NavigationViewInterface(this), true, NIP::SCROLL);
    addSubInterface("9", FluentIcon(FIT::APPLICATION), "窗口", new WindowInterface(this), true, NIP::SCROLL);
    addSubInterface("10", FluentIcon(FIT::SCROLL), "滚动", new ScrollInterface(this), true, NIP::SCROLL);
    addSubInterface("11", FluentIcon(FIT::CHAT), "状态", new StatusInfoInterface(this), true, NIP::SCROLL);
    addSubInterface("12", FluentIcon(iconPath.arg("Text")), "文本", new TextInterface(this), true, NIP::SCROLL);
    addSubInterface("13", FluentIcon(iconPath.arg("Grid")), "视图", new ViewInterface(this), true, NIP::SCROLL);

    navigationInterface()->addSeparator(NIP::BOTTOM);
    addSubInterface("15", FluentIcon(FIT::SETTING), "设置", new SettingInterface(this), true, NIP::BOTTOM);

    qrouter->setDefaultRouteKey(stackedWidget(), "homeInterface");
    navigationInterface()->setCurrentItem("1");

    connect(this, &MainWindow::backRequested, this, [=](){
        qrouter->pop();
        navigationInterface()->setCurrentItem(QString::number(stackedWidget()->currentIndex() + 1));
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
