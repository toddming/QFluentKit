#include "MainWindow.h"

#include "QFluent/navigation/NavigationPanel.h"


#include "HomeInterface.h"
#include "IconInterface.h"
#include "BasicInputInterface.h"
#include "SettingInterface.h"

MainWindow::MainWindow()
{
    setWindowTitle("QFluentKit");
    setWindowIcon(QPixmap(":/res/example.png"));
    setMinimumSize(866, 600);

    addSubInterface("1", FluentIconType::HOME, "主页", new HomeInterface(this), true, NavigationType::TOP);
    addSubInterface("2", FluentIconType::EMOJI_TAB_SYMBOLS, "图标", new IconInterface(this), true, NavigationType::TOP);
    navigationInterface()->addSeparator();
    addSubInterface("3", FluentIconType::CHECKBOX, "基本输入", new BasicInputInterface(this), true, NavigationType::SCROLL);

    navigationInterface()->addSeparator(NavigationType::NavigationItemPosition::BOTTOM);
    addSubInterface("11", FluentIconType::SETTING, "设置", new SettingInterface(this), true, NavigationType::BOTTOM);

}
