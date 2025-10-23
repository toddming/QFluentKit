#include "MainWindow.h"

#include "QFluent/PushButton.h"

#include "IconInterface.h"
#include "HomeInterface.h"

MainWindow::MainWindow()
{
    setMinimumSize(866, 600);

    auto w = new HomeInterface(this);
    addSubInterface("1", IconType::FLuentIcon::HOME, "主页", w, true, NavigationType::NavigationItemPosition::TOP);

    auto w2 = new IconInterface(this);
    addSubInterface("2", IconType::FLuentIcon::HOME, "图标", w2, true, NavigationType::NavigationItemPosition::TOP);

}
