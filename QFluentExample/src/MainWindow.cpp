#include "MainWindow.h"

#include "QFluent/navigation/NavigationPanel.h"


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

MainWindow::MainWindow()
{
    setWindowTitle("QFluentKit");
    setWindowIcon(QPixmap(":/res/example.png"));
    setMinimumSize(866, 600);

    setWindowButtonFlags(getWindowButtonFlags() | AppBarType::RouteBackButtonHint);

    addSubInterface("1", FluentIconType::HOME, "主页", new HomeInterface(this), true, NavigationType::TOP);
    addSubInterface("2", FluentIconType::EMOJI_TAB_SYMBOLS, "图标", new IconInterface(this), true, NavigationType::TOP);
    navigationInterface()->addSeparator();
    addSubInterface("3", FluentIconType::CHECKBOX, "基本输入", new BasicInputInterface(this), true, NavigationType::SCROLL);
    addSubInterface("4", FluentIconType::DATE_TIME, "日期和时间", new DateTimeInputInterface(this), true, NavigationType::SCROLL);
    addSubInterface("5", FluentIconType::MESSAGE, "对话框", new DialogInputInterface(this), true, NavigationType::SCROLL);
    addSubInterface("6", FluentIconType::LAYOUT, "布局", new LayoutInterface(this), true, NavigationType::SCROLL);
    addSubInterface("7", FluentIconType::COPY, "菜单", new MenuInterface(this), true, NavigationType::SCROLL);
    addSubInterface("8", FluentIconType::MENU, "导航", new NavigationViewInterface(this), true, NavigationType::SCROLL);
    addSubInterface("9", FluentIconType::SCROLL, "滚动", new ScrollInterface(this), true, NavigationType::SCROLL);
    addSubInterface("10", FluentIconType::CHAT, "状态", new StatusInfoInterface(this), true, NavigationType::SCROLL);
    addSubInterface("11", FluentIconType::TILES, "文本", new TextInterface(this), true, NavigationType::SCROLL);
    addSubInterface("12", FluentIconType::VIEW, "视图", new ViewInterface(this), true, NavigationType::SCROLL);

    navigationInterface()->addSeparator(NavigationType::NavigationItemPosition::BOTTOM);
    addSubInterface("13", FluentIconType::SETTING, "设置", new SettingInterface(this), true, NavigationType::BOTTOM);

    navigationInterface()->setCurrentItem("1");
}
