#include "MainWindow.h"

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

    navigationInterface()->addSeparator(Fluent::NavigationItemPosition::BOTTOM);
    addSubInterface("14", FluentIcon(Fluent::IconType::SETTING), "设置", new SettingInterface(this), true, Fluent::NavigationItemPosition::BOTTOM);

    navigationInterface()->setCurrentItem("1");

    NavigationAvatarWidget* avatar = navigationInterface()->avatarWidget();
    avatar->setName("Administrator");
    avatar->setAvatar(QImage(":/res/avatar.png"));
}
