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
    setMinimumSize(1024, 600);

    setWindowButtonFlags(getWindowButtonFlags() | AppBarType::RouteBackButtonHint);

    int theme = ConfigManager::instance().getValue("Window/theme", 0).toInt();
    Theme::instance()->setTheme(theme == 0 ? ThemeType::DARK : ThemeType::LIGHT);

    navigationInterface()->setExpandWidth(200);

    QString iconPath = QString(":/res/icons/%1_{color}.svg");
    addSubInterface("1", FluentIcon(FluentIconType::HOME), "主页", new HomeInterface(this), true, NavigationType::TOP);
    addSubInterface("2", FluentIcon(FluentIconType::EMOJI_TAB_SYMBOLS), "图标", new IconInterface(this), true, NavigationType::TOP);
    navigationInterface()->addSeparator();
    addSubInterface("3", FluentIcon(FluentIconType::CHECKBOX), "基本输入", new BasicInputInterface(this), true, NavigationType::SCROLL);
    addSubInterface("4", FluentIcon(FluentIconType::DATE_TIME), "日期和时间", new DateTimeInputInterface(this), true, NavigationType::SCROLL);
    addSubInterface("5", FluentIcon(FluentIconType::MESSAGE), "对话框", new DialogInputInterface(this), true, NavigationType::SCROLL);
    addSubInterface("6", FluentIcon(FluentIconType::LAYOUT), "布局", new LayoutInterface(this), true, NavigationType::SCROLL);
    addSubInterface("7", FluentIcon(iconPath.arg("Menu")), "菜单", new MenuInterface(this), true, NavigationType::SCROLL);
    addSubInterface("8", FluentIcon(FluentIconType::MENU), "导航", new NavigationViewInterface(this), true, NavigationType::SCROLL);
    addSubInterface("9", FluentIcon(FluentIconType::APPLICATION), "窗口", new WindowInterface(this), true, NavigationType::SCROLL);
    addSubInterface("10", FluentIcon(FluentIconType::SCROLL), "滚动", new ScrollInterface(this), true, NavigationType::SCROLL);
    addSubInterface("11", FluentIcon(FluentIconType::CHAT), "状态", new StatusInfoInterface(this), true, NavigationType::SCROLL);
    addSubInterface("12", FluentIcon(iconPath.arg("Text")), "文本", new TextInterface(this), true, NavigationType::SCROLL);
    addSubInterface("13", FluentIcon(iconPath.arg("Grid")), "视图", new ViewInterface(this), true, NavigationType::SCROLL);

    navigationInterface()->addSeparator(NavigationType::NavigationItemPosition::BOTTOM);
    addSubInterface("14", FluentIcon(FluentIconType::SETTING), "设置", new SettingInterface(this), true, NavigationType::BOTTOM);

    navigationInterface()->setCurrentItem("1");

    NavigationAvatarWidget* avatar = navigationInterface()->avatarWidget();
    avatar->setName("Administrator");
    avatar->setAvatar(QImage(":/res/avatar.png"));
}
